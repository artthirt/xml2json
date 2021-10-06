#ifdef WITH_QT
#endif

#include <string>
#include <iostream>
#include <fstream>

#include "xml2jsonqt.h"

using namespace std;

void parseXml(const string &input, const string &output)
{
#ifdef WITH_QT
    Xml2JsonQt cnv;
    cnv.parseXml(input, output);
#else
#endif
}


int main(int argc, char *argv[])
{
    string input = "input.xml";
    string output = "output.json";

    if(argc < 2){
        cout << "Usage: xml2json input.xml [output.json]\n";
        return 0;
    }

    input = argv[1];
    if(argc > 2){
        output = argv[2];
    }

    parseXml(input, output);

    return 0;
}
