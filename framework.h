#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <QObject>
#include <QAction>

class FrameWork : public QObject
{
    Q_OBJECT

public:
    FrameWork();

    static FrameWork *inst();

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
