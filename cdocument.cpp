#include "cdocument.h"

CDocument::CDocument()
{
    m_fileName = "";
}

void CDocument::setModel(ListModel *model)
{
    m_model = model;
}

void CDocument::setName(const QString &name)
{
    m_name = name;
}

void CDocument::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

ListModel *CDocument::getModel() const
{
    return m_model;
}

QString CDocument::getName() const
{
    return m_name;
}

QString CDocument::getFileName() const
{
    return m_fileName;
}
