#include "curllabel.h"

CUrlLabel::CUrlLabel(QWidget *parent)
    : QTextBrowser(parent)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Active,   QPalette::Text, pal.color(QPalette::Active,   QPalette::Foreground));
    pal.setBrush(QPalette::Active,   QPalette::Base, pal.brush(QPalette::Active,   QPalette::Background));
    pal.setColor(QPalette::Inactive, QPalette::Text, pal.color(QPalette::Inactive, QPalette::Foreground));
    pal.setBrush(QPalette::Inactive, QPalette::Base, pal.brush(QPalette::Inactive, QPalette::Background));
    pal.setColor(QPalette::Disabled, QPalette::Text, pal.color(QPalette::Disabled, QPalette::Foreground));
    pal.setBrush(QPalette::Disabled, QPalette::Base, pal.brush(QPalette::Disabled, QPalette::Background));
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
