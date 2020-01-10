#include "framework.h"

#include <QSettings>
#include <QFileDialog>

FrameWork *FrameWork::s_inst = nullptr;
const int maxFileNr = 4;

FrameWork::FrameWork()
{

}

FrameWork *FrameWork::inst()
{
    if(!s_inst) {
        s_inst = new FrameWork();
    }
    return s_inst;
}

void FrameWork::updateRecentActionList() {
    QSettings settings;
    QStringList recentFilePaths =
            settings.value("cache/recentFiles").toStringList();

    auto itEnd = 0;
    if(recentFilePaths.size() <= maxFileNr)
        itEnd = recentFilePaths.size();
    else
        itEnd = maxFileNr;

    for (auto i = 0; i < itEnd; ++i) {
        QString strippedName = QFileInfo(recentFilePaths.at(i)).fileName();
        recentFileActionList.at(i)->setText(strippedName);
        recentFileActionList.at(i)->setData(recentFilePaths.at(i));
        recentFileActionList.at(i)->setVisible(true);
    }

    for (auto i = itEnd; i < maxFileNr; ++i)
        recentFileActionList.at(i)->setVisible(false);
}

void FrameWork::addToRecentFiles(const QString &filePath){
//    setWindowFilePath(filePath);

    QSettings settings;
    QStringList recentFilePaths =
            settings.value("cache/recentFiles").toStringList();
    recentFilePaths.removeAll(filePath);
    recentFilePaths.prepend(filePath);
    while (recentFilePaths.size() > maxFileNr)
        recentFilePaths.removeLast();
    settings.setValue("cache/recentFiles", recentFilePaths);

    // see note
    updateRecentActionList();
}

int FrameWork::getMaxNumberOfRecentFiles() const
{
    return maxFileNr;
}

void FrameWork::addActionToRecentFiles(QAction *action)
{
    recentFileActionList.append(action);
}

QAction* FrameWork::getActionFromRecentFiles(int number) const
{
    return recentFileActionList.at(number);
}
