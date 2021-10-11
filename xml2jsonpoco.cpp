#include "xml2jsonpoco.h"

#ifdef WITH_POCO

using namespace std;

#include <Poco/XML/XML.h>
#include <Poco/XML/Content.h>
#include <Poco/SAX/Attributes.h>
#include <Poco/SAX/ContentHandler.h>
#include <Poco/SAX/SAXParser.h>
#include <Poco/JSON/JSON.h>

#include <algorithm>
#include <queue>
#include <mutex>
#include <stack>
#include <fstream>
#include <thread>

#include <jsoncpp/json/json.h>

#include "common.h"

#define TEST

class Xml2JsonPocoPrivate: public Poco::XML::ContentHandler{
public:

    void parseXml(const std::string &input, const std::string &output)
    {
        thread parser([this, input, output](){
            parseXmlThr(input, output);
        });
        thread writer([this, output](){
           writeJson(output);
        });
        parser.join();
        writer.join();
    }

    void parseXmlThr(const std::string &input, const std::string &output)
    {
        Poco::XML::SAXParser parser;
        parser.setContentHandler(this);

        try{
            parser.parse(input);
        }catch(...){

        }
        mDone = true;
    }

    void writeJson(const string& output){
        Json::Value root;

#ifdef TEST
        stack <vmap> _stack;
        _stack.push(vmap());
#endif

        stack < Json::Value> jstack;
        jstack.push(Json::Value());

        while(!mDone ||  !mQueue.empty()){
            if(mQueue.empty()){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            mMutex.lock();
            auto val = mQueue.front();
            mQueue.pop();
            mMutex.unlock();

            if(val.state == Val::IN){
#ifdef TEST
                vmap m;
                _stack.push(m);
#endif
                Json::Value j;
                jstack.push(j);
            }
            if(val.state == Val::OUT){
#ifdef TEST
                /// test code
                vmap t = _stack.top();
                _stack.pop();
                vmap& tn = _stack.top();

                if(!val.attrs.empty()){
                    t.insert(val.attrs.begin(), val.attrs.end());
                }

                if(contains(tn, val.key) && !t.empty()){
                    auto obj = tn[val.key];
                    vlist lst;
                    lst.push_back(obj);
                    lst.push_back(t);
                    tn[val.key] = lst;
                }else{
                    tn[val.key] = t;
                }
                /// end test code
#endif
                auto jt = jstack.top();
                jstack.pop();
                auto& jtn = jstack.top();

                if(!val.attrs.empty()){
                    for(auto it: val.attrs){
                        jt[it.first] = it.second.toText();
                    }
                }

                if(!jtn[val.key]){
                    jtn[val.key] = jt;
                }else{
                    // if key exists then need to create array
                    auto obj = jtn[val.key];
                    if(obj.isArray()){
                        obj[obj.size()] = jt;
                    }else{
                        Json::Value arr;
                        arr[0].copy(obj);
                        arr[1] = jt;
                        obj = arr;
                    }
                    jtn[val.key] = obj;
                }
            }

            if(!val.value.empty()){
#ifdef TEST
                vmap& t = _stack.top();
                t[val.key] = val.value;
#endif
                auto& jt = jstack.top();
                jt[val.key] = val.value;
            }else{

            }

//            printf("%d: %s, %s, %s\n", Val.depth, Val.key.c_str(), Val.value.c_str(), parent.c_str());
        }

        root = jstack.top();

        std::ofstream file_id;
        file_id.open(output);

        Json::StyledWriter writer;
        file_id << writer.write(root);
        file_id.close();

        printf("\n");
    }

private:
    int mDepth = 0;
    string mNode;
    string mValue;
    queue< Val > mQueue;
    mutex mMutex;
    stack<vmap> mAttrs;
    bool mDone = false;

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
    void startElement(const Poco::XML::XMLString &uri, const Poco::XML::XMLString &localName,
                      const Poco::XML::XMLString &qname, const Poco::XML::Attributes &attrList)
    {
        mNode = localName;
        {
            lock_guard<mutex> lock(mMutex);
            mQueue.push({mNode, "", mDepth, Val::IN}); // go deeper
        }
        vmap Attrs;
        for(int i = 0; i < attrList.getLength(); ++i){
            string key = attrList.getLocalName(i);
            string value = attrList.getValue(i);
            Attrs[key] = value;
        }
        mAttrs.push(Attrs);
        mDepth++;
    }
    void endElement(const Poco::XML::XMLString &uri, const Poco::XML::XMLString &localName, const Poco::XML::XMLString &qname)
    {
        mNode = localName;
        mDepth--;
        printf("%d: %s %s\n", mDepth, mNode.c_str(), mValue.c_str());
        vmap Attrs = mAttrs.top();
        mAttrs.pop();
        {
            lock_guard<mutex> lock(mMutex);
            mQueue.push({mNode, mValue, mDepth, Val::OUT, Attrs}); // out of children node
        }
    }
    void characters(const Poco::XML::XMLChar ch[], int start, int length)
    {
        mValue.resize(length);
        std::copy((char*)ch + start, (char*)ch + start + length, &mValue[0]);
        trim(mValue);
    }
    void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length) { }
    void processingInstruction(const Poco::XML::XMLString &target, const Poco::XML::XMLString &data) { }
    void startPrefixMapping(const Poco::XML::XMLString &prefix, const Poco::XML::XMLString &uri) { }
    void endPrefixMapping(const Poco::XML::XMLString &prefix) { }
    void skippedEntity(const Poco::XML::XMLString &name) { }
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
