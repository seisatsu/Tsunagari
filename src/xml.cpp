#include "xml.h"

std::string readXmlAttribute(xmlNode* node, const std::string& attr)
{
	xmlChar* result = xmlGetProp(node, BAD_CAST(attr.c_str()));
	std::string retval = result ? (const char*)result : "";
	xmlFree(result);
	return retval;
}

std::string readXmlElement(xmlNode* node)
{
	xmlChar* result = xmlNodeGetContent(node);
	std::string retval = result ? (const char*)result : "";
	xmlFree(result);
	return retval;
}

