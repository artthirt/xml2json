#include "xml2jsonlibxml.h"

#include <iostream>
#include <queue>
#include <list>
#include <thread>
#include <functional>
#include <stack>
#include <fstream>
#include <mutex>

#if !defined(WITH_QT) && !defined(WITH_POCO)

#include <libxml2/libxml/xmlreader.h>
#include <jsoncpp/json/json.h>

#endif

#include "common.h"

using namespace std;

#if !defined(WITH_QT) && !defined(WITH_POCO)

class Xml2JsonLibxmlPrivate{
public:
    queue< Val > mQueue;
    bool mDone = false;
    int mCurrentDepth = 0;
    mutex mMutex;

    void processNode(xmlNodePtr node, vmap& params, int depth = 0, const std::string& parent = ""){
        string key, value, attrName, attrValue;
        vmap attrs;
        for(auto it = node; it; it = it->next){
            if(it->type == XML_ELEMENT_NODE){
                key = reinterpret_cast<const char*>(it->name);
                xmlChar* xval = xmlNodeListGetString(it->doc, it->children, 1);
                value = reinterpret_cast<const char*>(xval);
                xmlFree(xval);

                trim(value); // remove \n or \r

                if(!value.empty()){
#ifdef TEST
                    params[key] = value;
#endif
                    lock_guard<mutex> lock(mMutex);
                    mQueue.push({key, value, depth, Val::NOCHANGE});
                }

                xmlAttr* attribute = it->properties;
                while(attribute)
                {
                    attrName = reinterpret_cast<const char*>(attribute->name);
                    xmlChar* value = xmlNodeListGetString(it->doc, attribute->children, 1);
                    attrValue= reinterpret_cast<const char*>(value);
                    //do something with value
                    xmlFree(value);
                    attribute = attribute->next;
                    attrs[attrName] = attrValue;
                }
            }

            if(it->children && value.empty()){
                {
                    lock_guard<mutex> lock(mMutex);
                    mQueue.push({key, "", depth, Val::IN}); // go deeper
                }

                vmap params2;
                auto lst = params[key].toList();
                processNode(it->children, params2, depth + 1, key);

#ifdef TEST

                if(!attrs.empty()){
                    params2.insert(attrs.begin(), attrs.end());
                }

                if(depth != 0){
                    if(!params2.empty())
                        lst.push_back(params2);
                    if(!lst.empty()){
                        params[key] = lst;
                     }
                }else{
                    params[key] = params2;
                }
#endif
                {
                    lock_guard<mutex> lock(mMutex);
                    mQueue.push({key, "", depth, Val::OUT, attrs}); // out of children node
                }
            }
        }
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

                if(!Val.attrs.empty()){
                    t.insert(Val.attrs.begin(), Val.attrs.end());
                }

                if(contains(tn, Val.key) && !t.empty()){
                    auto obj = tn[Val.key];
                    vlist lst;
                    lst.push_back(obj);
                    lst.push_back(t);
                    tn[Val.key] = lst;
                }else{
                    tn[Val.key] = t;
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
                t[Val.key] = Val.value;
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

    void parseXmlThr(const std::string &input, const std::string &output)
    {
        xmlInitParser();
        auto doc = xmlParseFile(input.c_str());
        if(!doc){
            cout << "can not parse file\n";
            return;
        }

        auto root_element = xmlDocGetRootElement(doc);
        vmap params;
        processNode(root_element, params);

        xmlFreeDoc(doc);
        xmlCleanupParser();

        mDone = true;
    }

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
};

#endif

Xml2JsonLibxml::Xml2JsonLibxml()
{

}

void Xml2JsonLibxml::parseXml(const std::string &input, const std::string &output)
{
#if !defined(WITH_QT) && !defined(WITH_POCO)
    Xml2JsonLibxmlPrivate priv;
    priv.parseXml(input, output);
#endif
}
