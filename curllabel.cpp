#include "curllabel.h"

CUrlLabel::CUrlLabel(QWidget *parent)
    : QTextBrowser(parent)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Active,   QPalette::Text, pal.color(QPalette::Active,   QPalette::WindowText));
    pal.setBrush(QPalette::Active,   QPalette::Base, pal.brush(QPalette::Active,   QPalette::Window));
    pal.setColor(QPalette::Inactive, QPalette::Text, pal.color(QPalette::Inactive, QPalette::WindowText));
    pal.setBrush(QPalette::Inactive, QPalette::Base, pal.brush(QPalette::Inactive, QPalette::Window));
    pal.setColor(QPalette::Disabled, QPalette::Text, pal.color(QPalette::Disabled, QPalette::WindowText));
    pal.setBrush(QPalette::Disabled, QPalette::Base, pal.brush(QPalette::Disabled, QPalette::Window));
    setPalette(pal);

    setFocusPolicy(Qt::NoFocus);

//    setTextFormat(Qt::RichText);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}


CUrlLabel::~CUrlLabel()
{
}
