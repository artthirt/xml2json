#ifndef XML2JSONPOCO_H
#define XML2JSONPOCO_H

#include <string>

#ifdef WITH_POCO

class Xml2JsonPoco
{
public:
    Xml2JsonPoco();

    void parseXml(const std::string &input, const std::string &output);
};

#endif

#endif // XML2JSONPOCO_H
