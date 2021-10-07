#include "xml2jsonpoco.h"

#ifdef WITH_POCO

using namespace std;

#include <Poco/XML/XML.h>
#include <Poco/XML/Content.h>
#include <Poco/SAX/ContentHandler.h>
#include <Poco/SAX/SAXParser.h>
#include <Poco/JSON/JSON.h>

class Xml2JsonPocoPrivate: public Poco::XML::ContentHandler{
public:
    void parseXml(const std::string &input, const std::string &output)
    {
        Poco::XML::SAXParser parser;
        parser.setContentHandler(this);

        try{
            parser.parse(input);
        }catch(...){

        }
    }

private:


    // ContentHandler interface
public:
    void setDocumentLocator(const Poco::XML::Locator *loc)
    {
    }
    void startDocument()
    {
    }
    void endDocument()
    {
    }
    void startElement(const Poco::XML::XMLString &uri, const Poco::XML::XMLString &localName, const Poco::XML::XMLString &qname, const Poco::XML::Attributes &attrList)
    {
    }
    void endElement(const Poco::XML::XMLString &uri, const Poco::XML::XMLString &localName, const Poco::XML::XMLString &qname)
    {
    }
    void characters(const Poco::XML::XMLChar ch[], int start, int length)
    {
    }
    void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length)
    {
    }
    void processingInstruction(const Poco::XML::XMLString &target, const Poco::XML::XMLString &data)
    {
    }
    void startPrefixMapping(const Poco::XML::XMLString &prefix, const Poco::XML::XMLString &uri)
    {
    }
    void endPrefixMapping(const Poco::XML::XMLString &prefix)
    {
    }
    void skippedEntity(const Poco::XML::XMLString &name)
    {
    }
};


Xml2JsonPoco::Xml2JsonPoco()
{

}

void Xml2JsonPoco::parseXml(const std::string &input, const std::string &output)
{
    Xml2JsonPocoPrivate parser;
    parser.parseXml(input, output);
}

#endif
