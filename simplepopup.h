#ifndef SIMPLEPOPUP_H
#define SIMPLEPOPUP_H

#include <QWidget>

namespace Ui {
class SimplePopup;
}

class SimplePopup : public QWidget
{
    Q_OBJECT

public:
    explicit SimplePopup(QWidget *parent = nullptr);
    ~SimplePopup();

private:
    Ui::SimplePopup *ui;
};

#endif // SIMPLEPOPUP_H
