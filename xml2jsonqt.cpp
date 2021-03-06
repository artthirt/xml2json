#include "xml2jsonqt.h"

#include <iostream>
#include <string>

#ifdef WITH_QT

#include <QFile>
#include <QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QVariant>

using namespace std;

class Xml2JsonQtPrivate{
public:

    QVariantMap getAttrName(const QDomNamedNodeMap &attr)
    {
        QMap<QString, QVariant> res;
        for(int i = 0; i < attr.size(); ++i){
            QString n = attr.item(i).nodeName();
            QString v = attr.item(i).nodeValue();
            res[n] = QList<QVariant>() << v;
        }
        return res;
    }

    void parseDom(const QDomNode &node, QMap<QString, QVariant> &out)
    {
        QString nodeName;
        auto it = node.firstChild();

        while(!it.isNull()){
            nodeName = it.nodeName();
            auto attr = getAttrName(it.attributes());

            auto lst = out[nodeName].toList();

            if(!it.firstChild().isText()){
                QMap<QString, QVariant> params;
                parseDom(it, params);
                lst.append(params);
            }else{
                lst.append(it.firstChild().nodeValue());
            }
            if(!attr.empty()){
                if(lst.last().type() == QVariant::Map){
                    auto map = lst.last().toMap();
                    attr.insert(map);
                    lst.last() = attr;
                }
            }
            out[nodeName]= lst;
            it = it.nextSibling();
        }
    }

    void writeToJson(QJsonObject &json, const QString &key, const QVariant &val)
    {
        if(val.type() == QVariant::Map){
            writeToJson(json, val.toMap());
        }else{
            json[key] = val.toString();
        }
    }

    void writeToJson(QJsonObject &json, const QMap<QString, QVariant> &params)
    {
        if(params.isEmpty())
            return;

        for(auto it = params.begin(); it != params.end(); ++it){
            QString key = it.key();
            QVariant value = it.value();

            auto lst = value.toList();

            if(lst.size() == 1){
                auto val = lst[0];
                writeToJson(json, key, val);
            }else{
                QJsonArray arr;
                for(auto a: lst){
                    QJsonObject obj;
                    writeToJson(obj, key, a);
                    arr.append(QJsonValue(obj));
                }
                json[key] = arr;
            }
        }
    }

};

void Xml2JsonQt::parseXml(const string &input, const string &output)
{
    Xml2JsonQtPrivate xml2json;
    QDomDocument doc;
    QFile file(QString::fromStdString(input));
    if(!file.open(QIODevice::ReadOnly)){
        cout << "File not open\n";
        return;
    }
    QString errorMsg;
    int errorLine, errorColumn;
    if(!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn)){
        cout << errorMsg.toStdString() << ": "
                 << errorLine << ", "
                  << errorColumn << endl;
        return;
    }
    file.close();

    QMap<QString, QVariant> res;
    xml2json.parseDom(doc, res);

    if(res.empty())
        return;

    QJsonObject json;
    xml2json.writeToJson(json, res);
    if(!json.empty()){
        QJsonDocument doc(json);

        QFile out(QString::fromStdString(output));
        out.open(QIODevice::WriteOnly);
        out.write(doc.toJson(QJsonDocument::Indented));
        out.close();
    }
}

#endif
