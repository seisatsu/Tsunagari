/******************************
** Tsunagari Tile Engine     **
** sprite.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "log.h"
#include "resourcer.h"
#include "sprite.h"

Sprite::Sprite(Resourcer* rc, const std::string descriptor)
	: rc(rc), img(NULL), descriptor(descriptor)
{
	c.x = c.y = c.z = 0;
}

Sprite::~Sprite()
{
	delete img;
}

/**
 * Try to load in descriptor.
 */
bool Sprite::processDescriptor()
{
	xmlChar* str;

	xmlDoc* doc = rc->getXMLDoc(descriptor);
	if (!doc)
		return false;
	const xmlNode* root = xmlDocGetRootElement(doc);
	if (!root) {
		xmlFreeDoc(doc);
		return false;
	}
	xmlNode* node = root->xmlChildrenNode; // <sprite>

	node = node->xmlChildrenNode; // decend into children of <sprite>
	for (; node != NULL; node = node->next) {
		if (!xmlStrncmp(node->name, BAD_CAST("sheet"), 6)) {
			str = xmlNodeGetContent(node);
			xml.sheet = (char*)str;

			str = xmlGetProp(node, BAD_CAST("tilesizex"));
			xml.tilesize.x = atol((char*)str); // atol

			str = xmlGetProp(node, BAD_CAST("tilesizey"));
			xml.tilesize.y = atol((char*)str); // atol
		}
		if (!xmlStrncmp(node->name, BAD_CAST("phases"), 7) &&
				!processPhases(node)) {
			xmlFreeDoc(doc);
			return false;
		}
	}
	xmlFreeDoc(doc);
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

	xmlChar* pos = xmlGetProp(phase, BAD_CAST("pos"));
	xmlChar* speed = xmlGetProp(phase, BAD_CAST("speed"));

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
		const uint32_t value = atol((char*)pos); // atol
		xml.phases[key] = value;
	}
	else { // speed
		// TODO: Load animated sprites
		// Load <member> subelements
	}
	return true;
}

bool Sprite::init()
{
	if (!processDescriptor())
		return false;
	img = rc->getImage(xml.sheet);
	return img != NULL;
}

void Sprite::draw() const
{
	img->draw(c.x, c.y, 0);
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

void Sprite::moveByPixel(coord_t dc)
{
	c.x += dc.x;
	c.y += dc.y;
	c.z += dc.z;
}

void Sprite::moveByTile(coord_t dc)
{
	c.x += dc.x * img->width();
	c.y += dc.y * img->height();
	c.z += dc.z; // XXX: Z-buffers
}

void Sprite::setCoordsByPixel(coord_t coords)
{
	c = coords;
}

void Sprite::setCoordsByTile(coord_t coords)
{
	c = coords;
	c.x *= img->width();
	c.y *= img->height();
	// XXX: set c.z when we have Z-buffers
}

