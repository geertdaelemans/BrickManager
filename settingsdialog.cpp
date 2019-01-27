#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtCore>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    // Disable "What's This" button on Title bar
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->setupUi(this);

    ui->lblGeneral->setText("No General Settings yet...");

    // Retrieve OAuth1 credentials
    QSettings settings;
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
    settings.setValue("credentials/consumerKey", ui->consumerKey->text());
    settings.setValue("credentials/consumerSecret", ui->consumerSecret->text());
    settings.setValue("credentials/tokenValue", ui->tokenValue->text());
    settings.setValue("credentials/tokenSecret", ui->tokenSecret->text());
}
