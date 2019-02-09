#include "simplepopup.h"
#include "ui_simplepopup.h"

SimplePopup::SimplePopup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimplePopup)
{
    ui->setupUi(this);
}

SimplePopup::~SimplePopup()
{
    delete ui;
}
