/******************************
** Tsunagari Tile Engine     **
** common.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <sstream>

#include "common.h"

coord_t coord(long x, long y, long z)
{
	coord_t c;
	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

cube_t cube(long x1, long y1, long z1,
            long x2, long y2, long z2)
{
	cube_t c;
	c.x1 = x1;
	c.y1 = y1;
	c.z1 = z1;
	c.x2 = x2;
	c.y2 = y2;
	c.z2 = z2;
	return c;
}

bool parseBool(const std::string& s)
{
	return s == "true" || 
	    s == "True" || 
	    s == "TRUE" || 
	    s == "yes" || 
	    s == "Yes" || 
	    s == "YES" || 
	    s == "1";
}

std::vector<std::string> splitStr(std::string str, const std::string& delimiter)
{
	std::vector<std::string> strlist;
	size_t pos;
	
	pos = str.find(delimiter);
	
	while (pos != std::string::npos) {
		if (pos != std::string::npos || pos+1 != str.size()) {
			if (str.size() != 0 && pos != 0) // Don't save empty strings
				strlist.push_back(str.substr(0, pos)); // Save
			str = str.substr(pos+delimiter.size()); // Cut delimiter
		}
		pos = str.find(delimiter);
	}

	if (pos == std::string::npos && str.size() != 0)
		strlist.push_back(str);

	return strlist;
}

std::string itostr(long in)
{
	std::stringstream out;
	out << in;
	return out.str();
}

std::string readXmlAttribute(xmlNode* node, const std::string attr)
{
	xmlChar* result = xmlGetProp(node, BAD_CAST(attr.c_str()));
	std::string retval = result ? (const char*)result : "";
	free(result);
	return retval;
}

std::string readXmlElement(xmlNode* node)
{
	xmlChar* result = xmlNodeGetContent(node);
	std::string retval = result ? (const char*)result : "";
	free(result);
	return retval;
}

