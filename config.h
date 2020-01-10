#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QSettings>

class Config
{
public:
    Config();

    static Config *inst();

    void setFilterBrickArmsColors(const bool value);
    bool includeBrickArmsColors() const;

    void setFilterModulexColors(const bool value);
    bool includeModulexColors() const;

    void setDocumentDir(const QString &directory);
    QString getDocumentDir() const;

    void setRecentFiles(const QStringList &files);
    QStringList getRecentFiles() const;

private:
    static Config *s_inst;
    QSettings m_settings;
};

#endif // CONFIG_H
