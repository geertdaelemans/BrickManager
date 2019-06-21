#ifndef EXPORTXML_H
#define EXPORTXML_H
#include <QWidget>

class ExportXml
{
public:
    ExportXml();
    static void SaveXMLFile(QString table, QString fileName);
    void ReadXMLFile();
};

#endif // EXPORTXML_H
