#include "ordersdialog.h"
#include "orderstablemodel.h"
#include "ui_ordersdialog.h"

#include <QMenu>
#include <QWidgetAction>

OrdersDialog::OrdersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrdersDialog)
{
    model = new OrdersTableModel(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->sort(1, Qt::DescendingOrder);

    ui->setupUi(this);
    ui->view->setModel(proxyModel);
    ui->view->horizontalHeader()->setSectionsMovable(true);
    ui->view->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    // Connect SLOT to context menu
    connect(ui->view->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), SLOT(slotCustomMenuRequested(QPoint)));

    model->updateOrders();
}

OrdersDialog::~OrdersDialog()
{
    delete ui;
}

void OrdersDialog::slotCustomMenuRequested(const QPoint pos) {
    QMenu *p_popUpMenu = new QMenu(this);

    // TODO: Configuration tab including order and visibility
    // TODO: p_popUpMenu->addAction(new QAction("Configure", this));
    // TODO: p_popUpMenu->addSeparator();

    // List all possible fields and select visibility
    auto metaEnum = QMetaEnum::fromType<OrdersDialog::Field>();
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        auto val = static_cast<Field>(metaEnum.value(i));
        QCheckBox *p_checkBox = new QCheckBox(p_popUpMenu);
        p_checkBox->setText(OrdersDialog::getHeader(val));
        p_checkBox->setObjectName(QString::number(i));
        p_checkBox->setChecked(OrdersDialog::getVisibilityColumn(val));
        QWidgetAction *p_checkableAction = new QWidgetAction(p_popUpMenu);
        p_checkableAction->setDefaultWidget(p_checkBox);
        p_popUpMenu->addAction(p_checkableAction);
        connect(p_checkBox, &QCheckBox::toggled, this, &OrdersDialog::setVisibilityFromCheckBox);
    }
    QPoint *p_position = new QPoint(pos.x(), pos.y());
    p_position->ry() -= 30; // Correct off set popup window
    p_popUpMenu->popup(ui->view->viewport()->mapToGlobal(*p_position));
};

void OrdersDialog::setVisibilityFromCheckBox() {
    auto *p_checkBox = qobject_cast<QCheckBox *>(sender());
    Field index = static_cast<OrdersDialog::Field>(p_checkBox->objectName().toInt());
    setVisibility(index, p_checkBox->isChecked());
}

bool OrdersDialog::getVisibilityColumn(Field field) {
    return columnVisibility.value(field);
}

void OrdersDialog::setVisibility(Field field) {
    bool visible = getVisibilityColumn(field);
    setVisibility(field, visible);
}

void OrdersDialog::setVisibility(Field field, bool visible) {
    if(visible) {
        ui->view->showColumn(field);
        columnVisibility[field] = true;
    } else {
        ui->view->hideColumn(field);
        columnVisibility[field] = false;
    }
}


void OrdersDialog::on_checkBoxFiled_stateChanged(int arg1)
{
    model->switchFiled(arg1);
    model->updateOrders();
}

void OrdersDialog::on_pushButton_clicked()
{
    QItemSelectionModel *select = ui->view->selectionModel();
    QModelIndexList rows = select->selectedRows(); // return selected row(s)
    QList<QString> list;
    foreach(QModelIndex index, rows) {
        list.append(ui->view->model()->data(ui->view->model()->index(index.row(), 0)).toString());
    }
    emit ordersSelected(list);
}

void OrdersDialog::on_view_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    OrdersDialog::on_pushButton_clicked();
}

QString OrdersDialog::getHeader(Field field) {
    switch (field) {
    case Field::OrderID:
        return QStringLiteral("Order Id");
    case Field::DateOrdered:
        return QStringLiteral("Date Ordered");
    case Field::SellerName:
        return QStringLiteral("Seller Name");
    case Field::StoreName:
        return QStringLiteral("Store Name");
    case Field::BuyerName:
        return QStringLiteral("Buyer Name");
    case Field::TotalCount:
        return QStringLiteral("Total Count");
    case Field::UniqueCount:
        return QStringLiteral("Unique Count");
    case Field::Status:
        return QStringLiteral("Status");
    case Field::PaymentMethod:
        return QStringLiteral("Payment Method");
    case Field::PaymentStatus:
        return QStringLiteral("Payment Status");
    case Field::PaymentDatePaid:
        return QStringLiteral("Date Paid");
    case Field::PaymentCurrencyCode:
        return QStringLiteral("Currency");
    case Field::CostSubtotal:
        return QStringLiteral("Subtotal");
    case Field::CostGrandTotal:
        return QStringLiteral("Grand Total");
    case Field::CostCurrencyCode:
        return QStringLiteral("Currency");
    }
    return "Error!";
}
