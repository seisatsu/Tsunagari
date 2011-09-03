/******************************
** Tsunagari Tile Engine     **
** xml.h                     **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef XML_H
#define XML_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

class XMLDoc {
public:
	bool init(const std::string& path,
	          const std::string& data,
	          const std::string& dtdFile);

	xmlNode* temporaryGetRoot() const;

	//! Equivalent to doc::unique();
	bool unique() const;

	//! Equivalent to doc::use_count().
	long use_count() const;

	//! Signifies whether document is has been initialized, parsed
	//! correctly, and is valid.
	operator bool() const;

private:
	boost::shared_ptr<xmlDoc> doc;
};

//! Get XML attribute (memory-safe).
std::string readXmlAttribute(xmlNode* node, const std::string& attr);

//! Get XML element content (memory-safe).
std::string readXmlElement(xmlNode* node);

#endif

