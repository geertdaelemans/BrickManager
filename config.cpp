#include "config.h"

Config *Config::s_inst = nullptr;

Config::Config()
{

}

Config *Config::inst()
{
    if(!s_inst) {
        s_inst = new Config();
    }
    return s_inst;
}

/**
 * Set filter for BrickArms colors
 * @param bool true: include / false: exclude
 */
void Config::setFilterBrickArmsColors(const bool value)
{
    m_settings.setValue("filter/includeBrickArmsColors", value);
}

/**
 * Get filter for BrickArms colors
 * @return bool true: include / false: exclude
 */
bool Config::includeBrickArmsColors() const
{
    return m_settings.value("filter/includeBrickArmsColors").toBool();
}

/**
 * Set filter for Modulex colors
 * @param bool true: include / false: exclude
 */
void Config::setFilterModulexColors(const bool value)
{
    m_settings.setValue("filter/includeModulexColors", value);
}

/**
 * Get filter for Modulex colors
 * @return bool true: include / false: exclude
 */
bool Config::includeModulexColors() const
{
    return m_settings.value("filter/includeModulexColors").toBool();
}

/**
 * Set the directory where documents will be stored
 * @param QStrinf directory
 */
void Config::setDocumentDir(const QString &directory)
{
    m_settings.setValue("paths/documentDir", directory);
}

/**
 * Get the directory where documents will be stored
 * @return QString directory
 */
QString Config::getDocumentDir() const
{
    return m_settings.value("paths/documentDir").toString();
}

/**
 * Set the list of recently visited documents
 * @param QStringList list of documents
 */
void Config::setRecentFiles(const QStringList &files)
{
    m_settings.setValue("cache/recentFiles", files);
}

/**
 * Get the list of recently visited documents
 * @return QStringList list of documents
 */
QStringList Config::getRecentFiles() const
{
    return m_settings.value("cache/recentFiles").toStringList();
}

