#include "exportxml.h"
#include "datamodel.h"

#include <QFile>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QtSql>

ExportXml::ExportXml()
{

}

//Writing and reading XML files with Qt - qxmlstreamwriter and qxmlstreamreader supported classes

void ExportXml::SaveXMLFile(QString table, QString fileName)
{ 
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(1);

    xmlWriter.writeStartDocument();

    xmlWriter.writeDTD("<!DOCTYPE BrickStockXML>");

    xmlWriter.writeStartElement("BrickStockXML");

    xmlWriter.writeStartElement("Inventory");

    DataModel* dataModel = new DataModel(Tables::brickstock);

    QSqlRecord record = QSqlDatabase::database("tempDatabase").record(table);
    int numberOfFields = record.count();
    QMap<int, QString> sqlDataField;

    for(int i = 0; i < numberOfFields; i++) {
        sqlDataField[i] = dataModel->getSqlColumnName(record.fieldName(i));
    }

    QSqlQuery q(QSqlDatabase::database("tempDatabase"));

    if (!q.prepare(QString("SELECT * FROM '%1'").arg(table)))
        qDebug() << "Error finding table.";
    q.exec();
    while(q.next()) {
        xmlWriter.writeStartElement("Item");
        for (int i = 0; i < numberOfFields; i++) {
            if(q.value(i).toString() != "") {
                xmlWriter.writeTextElement(sqlDataField[i], q.value(i).toString());
            }
        }
        xmlWriter.writeEndElement(); // Item
    }
    q.finish();

    xmlWriter.writeEndElement(); // Inventory

    xmlWriter.writeStartElement("GuiState");
    xmlWriter.writeAttribute("Application", "BrickStock");
    xmlWriter.writeAttribute("Version", "1");

    xmlWriter.writeStartElement("ItemView");

    xmlWriter.writeTextElement("ColumnOrder", "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30");
    xmlWriter.writeTextElement("ColumnWidths", "21,45,75,201,40,110,40,61,61,40,40,61,61,89,89,40,61,40,61,40,61,0,0,0,0,0,0,40,40,61,61");
    xmlWriter.writeTextElement("ColumnWidthsHidden", "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,61,61,61,61,75,40,40,40,61,61");
    xmlWriter.writeTextElement("SortColumn", "32");
    xmlWriter.writeTextElement("SortDirection", "A");

    xmlWriter.writeEndElement(); // ItemView

    xmlWriter.writeEndElement(); // GuiState

    xmlWriter.writeEndElement(); // BrickStockXML

    xmlWriter.writeEndDocument();

    file.close();
}



//void ExportXml::ReadXMLFile()
//{
//        QXmlStreamReader Rxml;

//        QString filename = QFileDialog::getOpenFileName(this,
//                                   tr("Open Xml"), ".",
//                                   tr("Xml files (*.xml)"));

//    QFile file(filename);
//        if (!file.open(QFile::ReadOnly | QFile::Text))
//    {
//        qDebug() << "Error: Cannot read file " << qPrintable(filename)
//                 << ": " << qPrintable(file.errorString());

//    }

//    Rxml.setDevice(&file);
//    Rxml.readNext();

//    while(!Rxml.atEnd())
//    {
//        if(Rxml.isStartElement())
//        {
//            if(Rxml.name() == "LAMPS")
//            {
//                Rxml.readNext();
//            }
//            else if(Rxml.name() == "LIGHT1")
//            {
//                while(!Rxml.atEnd())
//                            {
//                             if(Rxml.isEndElement())
//                             {
//                             Rxml.readNext();
//                             break;
//                             }
//                             else if(Rxml.isCharacters())
//                             {
//                             Rxml.readNext();
//                             }
//                             else if(Rxml.isStartElement())
//                             {
//                             if(Rxml.name() == "State")
//                             {
//                              ReadStateElement();
//                             }
//                             else if(Rxml.name() == "Room")
//                             {
//                              ReadRoomElement();
//                             }
//                             else if(Rxml.name() == "Potencial")
//                             {
//                                  ReadPotencialElement();
//                             }
//                             Rxml.readNext();
//                         }
//                         else
//                         {
//                         Rxml.readNext();
//                         }
//                    }
//            }
//        }
//    else
//    {
//        Rxml.readNext();
//    }

//    file.close();

//        if (Rxml.hasError())
//    {
//       qDebug() << "Error: Failed to parse file "
//                 << qPrintable(filename) << ": "
//                 << qPrintable(Rxml.errorString());
//        }
//    else if (file.error() != QFile::NoError)
//    {
//        qDebug() << "Error: Cannot read file " << qPrintable(filename)
//                  << ": " << qPrintable(file.errorString());
//    }
//}
//}


////Example for Room Element

//void ExportXml::ReadRoomElement()
//{
//    while(!Rxml.atEnd())
//    {
//        if(Rxml.isEndElement())
//        {
//            Rxml.readNext();;
//            break;
//        }
//        else if(Rxml.isStartElement())
//        {
//            QString roomelement = Rxml.readElementText();   //Get the xml value
//            Rxml.readNext();
//            break;
//        }
//        else if(Rxml.isCharacters())
//        {
//            Rxml.readNext();
//        }
//        else
//        {
//            Rxml.readNext();
//        }
//    }

//}
