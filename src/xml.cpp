/******************************
** Tsunagari Tile Engine     **
** xml.cpp                   **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "xml.h"


XMLNode::XMLNode()
{
}

XMLNode::XMLNode(XMLDoc* doc, xmlNode* node)
	: doc(doc), node(node)
{
}

XMLNode XMLNode::childrenNode() const
{
	return XMLNode(doc, node->xmlChildrenNode);
}

XMLNode XMLNode::next() const
{
	return XMLNode(doc, node->next);
}

bool XMLNode::is(const char* name) const
{
	return !xmlStrncmp(node->name, BAD_CAST(name), (int)strlen(name)+1);
}

std::string XMLNode::content() const
{
	xmlChar* content = xmlNodeGetContent(node);
	std::string s = content ? (const char*)content : "";
	xmlFree(content);
	return s;
}

bool XMLNode::intContent(int* i) const
{
	std::string s = content();
	if (!isInteger(s)) {
		Log::err(doc->path(), "expected integer");
		return false;
	}
	*i = atoi(s.c_str());
	return true;
}

bool XMLNode::doubleContent(double *d) const
{
	std::string s = content();
	if (!isDecimal(s)) {
		Log::err(doc->path(), "expected decimal");
		return false;
	}
	*d = atof(s.c_str());
	return true;
}

bool XMLNode::hasAttr(const std::string& name) const
{
	return xmlHasProp(node, BAD_CAST(name.c_str()));
}

std::string XMLNode::attr(const std::string& name) const
{
	xmlChar* content = xmlGetProp(node, BAD_CAST(name.c_str()));
	std::string s = content ? (const char*)content : "";
	xmlFree(content);
	return s;
}

bool XMLNode::intAttr(const std::string& name, int* i) const
{
	std::string s = attr(name);
	if (!isInteger(s)) {
		Log::err(doc->path(), "expected integer");
		return false;
	}
	*i = atoi(s.c_str());
	return true;
}

bool XMLNode::doubleAttr(const std::string& name, double* d) const
{
	std::string s = attr(name);
	if (!isDecimal(s)) {
		Log::err(doc->path(), "expected decimal");
		return false;
	}
	*d = atof(s.c_str());
	return true;
}

XMLNode::operator bool() const
{
	return node != NULL;
}


static void xmlErrorCb(void* pstrFilename, const char* msg, ...)
{

	const std::string* filename = (const std::string*)pstrFilename;
	char buf[512];
	va_list ap;

	va_start(ap, msg);
	snprintf(buf, sizeof(buf)-1, msg, va_arg(ap, char*));
	Log::err(*filename, buf);
	va_end(ap);
}

XMLDoc::XMLDoc()
{
}

bool XMLDoc::init(const std::string& path,
                  const std::string& data,
                  const std::string& dtdPath)
{
	this->path_ = path;

	xmlParserCtxt* pc = xmlNewParserCtxt();
	pc->vctxt.userData = (void*)&path;
	pc->vctxt.error = xmlErrorCb;

	// Parse the XML. Hand over our error callback fn.
	doc.reset(xmlCtxtReadMemory(pc, data.c_str(),
		(int)data.size(), NULL, NULL,
		XML_PARSE_NOBLANKS | XML_PARSE_NONET), xmlFreeDoc);
	xmlFreeParserCtxt(pc);
	if (!doc) {
		Log::err(path, "could not parse file");
		return false;
	}

	// Load up a Document Type Definition for validating the document.
	xmlDtd* dtd = xmlParseDTD(NULL, (const xmlChar*)dtdPath.c_str());
	if (!dtd) {
		doc.reset();
		Log::err(dtdPath, "file not found");
		return false;
	}

	// Assert the document is sane.
	xmlValidCtxt* vc = xmlNewValidCtxt();
	int valid = xmlValidateDtd(vc, doc.get(), dtd);
	xmlFreeValidCtxt(vc);
	xmlFreeDtd(dtd);

	if (!valid) {
		doc.reset();
		Log::err(path, "XML document does not follow DTD");
		return false;
	}

	return true;
}

XMLNode XMLDoc::root()
{
	return XMLNode(this, xmlDocGetRootElement(doc.get()));
}

xmlNode* XMLDoc::temporaryGetRoot() const
{
	return xmlDocGetRootElement(doc.get());
}

const std::string& XMLDoc::path() const
{
	return path_;
}

XMLDoc::operator bool() const
{
	return doc;
}

bool XMLDoc::unique() const
{
	return doc.unique();
}

long XMLDoc::use_count() const
{
	return doc.use_count();
}

