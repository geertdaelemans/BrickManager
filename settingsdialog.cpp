#include "settingsdialog.h"
#include "bricklink.h"
#include "ui_settingsdialog.h"

#include <QtCore>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    // Disable "What's This" button on Title bar
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->setupUi(this);

    QSettings settings;

    // GENERAL panel
    ui->checkBoxBrickArms->setChecked(settings.value("filter/includeBrickArmsColors").toBool());
    ui->checkBoxModulex->setChecked(settings.value("filter/includeModulexColors").toBool());

    // CONNECTION panel
    // Retrieve BrickLink Login credentials
    ui->userName->setText(settings.value("bricklinklogin/userName").toString());
    ui->passWord->setText(settings.value("bricklinklogin/passWord").toString());
    // Retrieve OAuth1 credentials
    ui->consumerKey->setText(settings.value("credentials/consumerKey").toString());
    ui->consumerSecret->setText(settings.value("credentials/consumerSecret").toString());
    ui->tokenValue->setText(settings.value("credentials/tokenValue").toString());
    ui->tokenSecret->setText(settings.value("credentials/tokenSecret").toString());

    ui->tabWidget->setCurrentIndex(0);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted()
{
    QSettings settings;
    settings.setValue("filter/includeBrickArmsColors", ui->checkBoxBrickArms->checkState());
    settings.setValue("filter/includeModulexColors", ui->checkBoxModulex->checkState());
    settings.setValue("bricklinklogin/userName", ui->userName->text());
    settings.setValue("bricklinklogin/passWord", ui->passWord->text());
    settings.setValue("credentials/consumerKey", ui->consumerKey->text());
    settings.setValue("credentials/consumerSecret", ui->consumerSecret->text());
    settings.setValue("credentials/tokenValue", ui->tokenValue->text());
    settings.setValue("credentials/tokenSecret", ui->tokenSecret->text());
}

void SettingsDialog::on_btnCheckConnection_clicked()
{
    this->on_buttonBox_accepted();
    BrickLink brickLink;
    if(brickLink.checkConnection(this)) {
        QMessageBox::information(this, "Connection", "Connection validated.");
    }
}
