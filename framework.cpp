#include "framework.h"
#include "sqldatabase.h"
#include "datamodel.h"
#include "listmodel.h"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDomDocument>

FrameWork *FrameWork::s_inst = nullptr;
const int maxFileNr = 4;

FrameWork::FrameWork()
{

}

FrameWork *FrameWork::inst()
{
    if(!s_inst) {
        s_inst = new FrameWork();
    }
    return s_inst;
}

CDocument *FrameWork::loadFile(QWidget* parent, const QString &fileName)
{
    QFile file(fileName);
    if(!fileName.isEmpty() && !fileName.isNull()) {
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(parent, "error", file.errorString());
            return nullptr;
        }

        // Add fileName to list of recent files
        FrameWork::inst()->addToRecentFiles(fileName);

        // Get file information
        QFileInfo info(file);
        QString tableName = info.baseName();

        //The QDomDocument class represents an XML document.
        QDomDocument xmlInventory;

        // Set data into the QDomDocument before processing
        xmlInventory.setContent(&file);
        file.close();

        // Extract the root markup
        QDomElement root = xmlInventory.documentElement();              // BrickStockXML
        QDomElement inventory = root.firstChild().toElement();          // Inventory
        QDomElement item = inventory.firstChild().toElement();          // Item

        // Prepare data model
        QString sqlTableName = SqlDatabase::getUniqueTableName(&tableName);
        DataModel *p_dataModel = new DataModel(Tables::brickstock, sqlTableName);
        p_dataModel->initiateSqlTableAuto("tempDatabase");

        // Read each child of the Inventory node
        while (!item.isNull()) {

            // Prepare fields
            QMap<QString, QVariant> fields;
            if (item.tagName() == "Item") {
                QDomElement field = item.firstChild().toElement();
                // Read Name and value
                while (!field.isNull()) {
                    fields[field.tagName()] = field.firstChild().toText().data();
                    if (fields[field.tagName()].isNull()) {
                        fields[field.tagName()] = QVariant(1);
                    }
                    // Next field
                    field = field.nextSibling().toElement();
                }
            }

            // Set original quantity to current quantity, if not already set
            if (!fields["OrigQty"].isValid())
                fields["OrigQty"] = fields["Qty"];

            // Set original unit price to current unit price, if not already set
            if (!fields["OrigPrice"].isValid())
                fields["OrigPrice"] = fields["Price"];

            // Add collected fields to SQL database
            QSqlError error = p_dataModel->addItemToTable(fields);

            // Next sibling
            item = item.nextSibling().toElement();
        }
        ListModel *listModel = new ListModel(parent, p_dataModel, QSqlDatabase::database("tempDatabase"));
        CDocument *doc = new CDocument();
        doc->setName(tableName);
        doc->setModel(listModel);
        doc->setFileName(fileName);
        return doc;
    }
    return nullptr;
}


void FrameWork::updateRecentActionList() {
    QSettings settings;
    QStringList recentFilePaths =
            settings.value("cache/recentFiles").toStringList();

    auto itEnd = 0;
    if(recentFilePaths.size() <= maxFileNr)
        itEnd = recentFilePaths.size();
    else
        itEnd = maxFileNr;

    for (auto i = 0; i < itEnd; ++i) {
        QString strippedName = QFileInfo(recentFilePaths.at(i)).fileName();
        recentFileActionList.at(i)->setText(strippedName);
        recentFileActionList.at(i)->setData(recentFilePaths.at(i));
        recentFileActionList.at(i)->setVisible(true);
    }

    for (auto i = itEnd; i < maxFileNr; ++i)
        recentFileActionList.at(i)->setVisible(false);
}

void FrameWork::addToRecentFiles(const QString &filePath){
//    setWindowFilePath(filePath);

    QSettings settings;
    QStringList recentFilePaths =
            settings.value("cache/recentFiles").toStringList();
    recentFilePaths.removeAll(filePath);
    recentFilePaths.prepend(filePath);
    while (recentFilePaths.size() > maxFileNr)
        recentFilePaths.removeLast();
    settings.setValue("cache/recentFiles", recentFilePaths);

    // see note
    updateRecentActionList();
}

int FrameWork::getMaxNumberOfRecentFiles() const
{
    return maxFileNr;
}

void FrameWork::addActionToRecentFiles(QAction *action)
{
    recentFileActionList.append(action);
}

QAction* FrameWork::getActionFromRecentFiles(int number) const
{
    return recentFileActionList.at(number);
}
