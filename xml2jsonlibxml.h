#ifndef XML2JSONLIBXML_H
#define XML2JSONLIBXML_H

#include <string>

class Xml2JsonLibxml
{
public:
    Xml2JsonLibxml();

    void parseXml(const std::string &input, const std::string &output);
};

#endif // XML2JSONLIBXML_H
