#include "log.h"
#include "xml.h"

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

bool XMLDoc::init(const std::string& path,
                  const std::string& data,
                  const std::string& dtdPath)
{
	xmlParserCtxt* pc = xmlNewParserCtxt();
	pc->vctxt.userData = (void*)&path;
	pc->vctxt.error = xmlErrorCb;

	// Parse the XML. Hand over our error callback fn.
	doc.reset(xmlCtxtReadMemory(pc, data.c_str(),
		(int)data.size(), NULL, NULL,
		XML_PARSE_NOBLANKS | XML_PARSE_NONET), xmlFreeDoc);
	xmlFreeParserCtxt(pc);
	if (!doc) {
		Log::err(path, "Could not parse file");
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

xmlNode* XMLDoc::temporaryGetRoot() const
{
	return xmlDocGetRootElement(doc.get());
}

XMLDoc::operator bool() const
{
	return doc;
}

long XMLDoc::use_count() const
{
	return doc.use_count();
}

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

