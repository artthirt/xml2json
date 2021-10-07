#ifndef COMMON_H
#define COMMON_H

#include <memory>
#include <string>
#include <map>
#include <list>

class Variant{
public:
    enum Type {EMPTY, STRING, MAP, LIST};

    typedef std::map< std::string, Variant> vmap;
    typedef std::list< Variant > vlist;

    Variant(){}
    Variant(const std::string &val): mText(val), mType(STRING){}
    Variant(const vmap &val): mMap(val), mType(MAP){}
    Variant(const vlist val): mList(val), mType(LIST){}

    std::string toText() const{
        return mText;
    }
    vmap toMap() const{
        return mMap;
    }
    vlist toList() const{
        return mList;
    }

    Variant& operator= (const std::string& val){
        clear();
        mType = STRING;
        mText = val;
        return *this;
    }
    Variant& operator= (const char* val){
        clear();
        mType = STRING;
        mText = val;
        return *this;
    }
    Variant& operator= (const vmap& val){
        clear();
        mType = MAP;
        mMap = val;
        return *this;
    }
    Variant& operator= (const vlist& val){
        clear();
        mType = LIST;
        mList = val;
        return *this;
    }
    Type type() const{
        return mType;
    }
    void clear(){
        mType = EMPTY;
        mMap.clear();
        mList.clear();
        mText = "";
    }
private:
    Type mType = EMPTY;
    std::string mText;
    vmap mMap;
    vlist mList;

};

typedef Variant::vmap vmap;
typedef Variant::vlist vlist;

#include <algorithm>

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline std::string trimmed(const std::string &val)
{
    std::string s = val;
    ltrim(s);
    rtrim(s);
    return s;
}

static inline std::string trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
    return s;
}

template <typename T, typename N>
bool contains(std::map<T, N>& In, const T& key)
{
    return In.find(key) != In.end();
}

#endif // COMMON_H
