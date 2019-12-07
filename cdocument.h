#ifndef CDOCUMENT_H
#define CDOCUMENT_H

#include "listmodel.h"

class CDocument
{
public:
    CDocument();
    void setModel(ListModel *m_model);
    void setName(const QString &name);
    void setFileName(const QString &fileName);
    ListModel *getModel() const;
    QString getName() const;
    QString getFileName() const;

private:
    ListModel *m_model;
    QString m_name;
    QString m_fileName;
};

#endif // CDOCUMENT_H
