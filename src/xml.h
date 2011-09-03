/******************************
** Tsunagari Tile Engine     **
** xml.h                     **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef XML_H
#define XML_H

#include <string>

#include <libxml/tree.h>

class XMLDocument {
public:
	bool init(const std::string& path,
	          const std::string& data,
	          const std::string& dtdFile);

	xmlDoc* temporaryGetDoc() const;

private:
	xmlDoc* doc;
};

//! Get XML attribute (memory-safe).
std::string readXmlAttribute(xmlNode* node, const std::string& attr);

//! Get XML element content (memory-safe).
std::string readXmlElement(xmlNode* node);

#endif

