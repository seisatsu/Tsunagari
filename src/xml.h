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

#ifndef LIBXML_TREE_ENABLED
	#error Tree must be enabled in libxml2
#endif

class XMLDoc;

class XMLNode {
public:
	XMLNode();
	XMLNode(XMLDoc* doc, xmlNode* node);

	XMLNode childrenNode() const;
	XMLNode next() const;

	bool is(const char* name) const;

	std::string content() const;
	bool intContent(int* i) const;
	bool doubleContent(double* d) const;

	bool hasAttr(const std::string& name) const;
	std::string attr(const std::string& name) const;
	bool intAttr(const std::string& name, int* i) const;
	bool doubleAttr(const std::string& name, double* d) const;

	//! Whether this is a valid node (non-NULL).
	operator bool() const;

private:
	XMLDoc* doc;
	xmlNode* node;
};

class XMLDoc {
public:
	XMLDoc();
	bool init(const std::string& path,
	          const std::string& data,
	          const std::string& dtdFile);

	XMLNode root();
	xmlNode* temporaryGetRoot() const;
	const std::string& path() const;

	//! Equivalent to doc::unique();
	bool unique() const;

	//! Equivalent to doc::use_count().
	long use_count() const;

	//! Signifies whether document is has been initialized, parsed
	//! correctly, and is valid.
	operator bool() const;

private:
	boost::shared_ptr<xmlDoc> doc;
	std::string path_;
};

#endif

