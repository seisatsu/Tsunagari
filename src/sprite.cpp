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
	c.x = c.y = 0;
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
	
	const xmlNode* root = rc->getXMLDoc(descriptor);
	if (!root)
		return false;
	xmlNode* node = root->xmlChildrenNode; // <sprite>
	
	node = node->xmlChildrenNode; // decend into children of <sprite>
	while (node != NULL) {
		if (!xmlStrncmp(node->name, BAD_CAST("sheet"), 6)) {
			str = xmlNodeGetContent(node);
			xml.sheet = (char*)str;

			str = xmlGetProp(node, BAD_CAST("tilesizex"));
			xml.tilesize.x = atol((char*)str); // atol

			str = xmlGetProp(node, BAD_CAST("tilesizey"));
			xml.tilesize.y = atol((char*)str); // atol
		}
		if (!xmlStrncmp(node->name, BAD_CAST("phases"), 7)) {
			xmlNode* p = node->xmlChildrenNode;
			while (p != NULL) {
				if (!xmlStrncmp(p->name, BAD_CAST("phase"), 6)) { // ignore comments
					/* Each phase requires a 'name'. Additionally,
					 * one of either 'pos' or 'speed' is needed.
					 * If speed is used, we have sub-elements. We
					 * can't have both pos and speed.
					 */
					const std::string key = (char*)xmlGetProp(p, BAD_CAST("name"));

					xmlChar* pos = xmlGetProp(p, BAD_CAST("pos"));
					xmlChar* speed = xmlGetProp(p, BAD_CAST("speed"));

					if (pos && speed) {
						Log::err(descriptor, "pos and speed attributes in element phase are mutually exclusive");
						return false;
					}
					if (!pos && !speed) {
						Log::err(descriptor, "Must have one of pos or speed attributes in element phase");
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
				}
				p = p->next;
			}
		}
		node = node->next;
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

