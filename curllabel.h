#ifndef CURLLABEL_H
#define CURLLABEL_H

#include <QWidget>
#include <QTextBrowser>

class CUrlLabel : public QTextBrowser
{
public:
    CUrlLabel(QWidget *parent);
    ~CUrlLabel();
};

#endif // CURLLABEL_H
