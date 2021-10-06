#ifndef XML2JSONQT_H
#define XML2JSONQT_H

#ifdef WITH_QT

#include <string>

class Xml2JsonQt{
public:
    void parseXml(const std::string &input, const std::string &output);
};

#endif

#endif // XML2JSONQT_H
