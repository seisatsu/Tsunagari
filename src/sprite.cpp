/******************************
** Tsunagari Tile Engine     **
** sprite.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Image.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "log.h"
#include "resourcer.h"
#include "sprite.h"

Sprite::Sprite(Resourcer* rc)
	: rc(rc)
{
	c.x = c.y = c.z = 0;
}

Sprite::~Sprite()
{
	boost::unordered_map<std::string, Gosu::Image*>::iterator it;
	for (it = imgs.begin(); it != imgs.end(); it++) {
		Gosu::Image* img = (*it).second;
		delete img;
	}
}

bool Sprite::init(const std::string& descriptor)
{
	this->descriptor = descriptor;
	return processDescriptor() && loadPhases();
}

void Sprite::draw() const
{
	img->draw((double)c.x, (double)c.y, (double)0);
}

bool Sprite::setPhase(const std::string& name)
{
	boost::unordered_map<std::string, Gosu::Image*>::iterator phase;
	phase = imgs.find(name);
	if (phase != imgs.end())
		img = (*phase).second;
	return phase != imgs.end();
}

coord_t Sprite::getCoordsByPixel()
{
	return c;
}

coord_t Sprite::getCoordsByTile()
{
	coord_t coords;
	coords.x = c.x / img->width();
	coords.y = c.y / img->height();
	coords.z = c.z; // XXX: revisit when we have Z-buffers
	return coords;
}

void Sprite::setCoordsByPixel(coord_t coords)
{
	c = coords;
}

void Sprite::setCoordsByTile(coord_t coords)
{
	// FIXME: use Area's tile width
	c = coords;
	c.x *= img->width();
	c.y *= img->height();
	// XXX: set c.z when we have Z-buffers
}

void Sprite::moveByPixel(coord_t dc)
{
	c.x += dc.x;
	c.y += dc.y;
	c.z += dc.z;
}

void Sprite::moveByTile(coord_t dc)
{
	// FIXME: use Area's tile width
	c.x += dc.x * img->width();
	c.y += dc.y * img->height();
	// XXX: set c.z when we have Z-buffers
}

/**
 * Try to load in descriptor.
 */
bool Sprite::processDescriptor()
{
	XMLDocRef doc = rc->getXMLDoc(descriptor, "dtd/sprite.dtd");
	if (!doc)
		return false;
	const xmlNode* root = xmlDocGetRootElement(doc.get()); // <sprite>
	if (!root)
		return false;
	xmlNode* node = root->xmlChildrenNode; // children of <sprite>

	xmlChar* str;
	for (; node != NULL; node = node->next) {
		if (!xmlStrncmp(node->name, BAD_CAST("sheet"), 6)) {
			str = xmlNodeGetContent(node);
			xml.sheet = (char*)str;

			str = xmlGetProp(node, BAD_CAST("tilesizex"));
			xml.tilesize.x = atol((char*)str); // atol

			str = xmlGetProp(node, BAD_CAST("tilesizey"));
			xml.tilesize.y = atol((char*)str); // atol
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("phases"), 7) &&
				!processPhases(node))
			return false;
	}
	return true;
}

bool Sprite::processPhases(xmlNode* phases)
{
	for (xmlNode* phase = phases->xmlChildrenNode; phase != NULL;
			phase = phase->next)
		if (!xmlStrncmp(phase->name, BAD_CAST("phase"), 6))
			if (!processPhase(phase))
				return false;
	return true;
}

bool Sprite::processPhase(xmlNode* phase)
{
	/* Each phase requires a 'name'. Additionally,
	 * one of either 'pos' or 'speed' is needed.
	 * If speed is used, we have sub-elements. We
	 * can't have both pos and speed.
	 */
	const std::string key = (char*)xmlGetProp(phase, BAD_CAST("name"));

	const xmlChar* pos = xmlGetProp(phase, BAD_CAST("pos"));
	const xmlChar* speed = xmlGetProp(phase, BAD_CAST("speed"));

	if (pos && speed) {
		Log::err(descriptor, "pos and speed attributes in "
				"element phase are mutually exclusive");
		return false;
	}
	if (!pos && !speed) {
		Log::err(descriptor, "must have pos or speed attribute "
			       "in element phase");
		return false;
	}

	if (pos) {
		const unsigned value = (unsigned)atoi((const char*)pos); // atol
		xml.phases[key] = value;
	}
	else { // speed
		// TODO: Load animated sprites
		// Load <member> subelements
	}

	return true;
}

bool Sprite::loadPhases()
{
	Gosu::Bitmap src;
	if (!rc->getBitmap(src, xml.sheet))
		return false;

	boost::unordered_map<std::string, unsigned>::iterator it;
	for (it = xml.phases.begin(); it != xml.phases.end(); it++) {
		const std::string& name = (*it).first;
		unsigned idx = (*it).second;
		Gosu::Image* image = loadImage(src, idx);
		imgs[name] = img = image;
	}
	return true;
}

Gosu::Image* Sprite::loadImage(const Gosu::Bitmap& src, unsigned pos)
{
	unsigned x = (unsigned)((xml.tilesize.x * pos) %
			src.width());
	unsigned y = (unsigned)((xml.tilesize.y * pos) /
			src.width() * xml.tilesize.y); // ???

	// FIXME: check for index out of bounds

	return rc->bitmapSection(src, x, y, (unsigned)xml.tilesize.x,
			(unsigned)xml.tilesize.y, true);
}

