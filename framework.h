#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <QObject>
#include <QAction>

#include "datamodel.h"
#include "cdocument.h"

class FrameWork : public QObject
{
    Q_OBJECT

public:
    FrameWork();

    static FrameWork *inst();

    CDocument *loadFile(QWidget *parent, const QString &fileName);

    void updateRecentActionList();
    void addToRecentFiles(const QString &filePath);
    void addActionToRecentFiles(QAction *action);
    int getMaxNumberOfRecentFiles() const;
    QAction *getActionFromRecentFiles(int number) const;

private:
    static FrameWork *s_inst;
    QList<QAction*> recentFileActionList;
};

#endif // FRAMEWORK_H
