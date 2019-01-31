#include "inventory.h"
#include "ui_inventory.h"

#include <QMenu>
#include <QCheckBox>
#include <QWidgetAction>

Inventory::Inventory(QWidget *parent, int orderID) :
    QWidget(parent),
    ui(new Ui::Inventory)
{
    model = new InventoryTableModel(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);

    ui->setupUi(this);
    ui->tableView->setModel(proxyModel);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    // Connect SLOT to context menu
    connect(ui->tableView->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), SLOT(slotCustomMenuRequested(QPoint)));

    model->updateParts(orderID);
}

Inventory::~Inventory()
{
    delete ui;
}

void Inventory::slotCustomMenuRequested(const QPoint pos) {
    QMenu *p_popUpMenu = new QMenu(this);

    // TODO: Configuration tab including order and visibility
    // TODO: p_popUpMenu->addAction(new QAction("Configure", this));
    // TODO: p_popUpMenu->addSeparator();

    // List all possible fields and select visibility
    auto metaEnum = QMetaEnum::fromType<Inventory::Field>();
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        auto val = static_cast<Field>(metaEnum.value(i));
        QCheckBox *p_checkBox = new QCheckBox(p_popUpMenu);
        p_checkBox->setText(Inventory::getHeader(val));
        p_checkBox->setObjectName(QString::number(i));
        p_checkBox->setChecked(Inventory::getVisibilityColumn(val));
        QWidgetAction *p_checkableAction = new QWidgetAction(p_popUpMenu);
        p_checkableAction->setDefaultWidget(p_checkBox);
        p_popUpMenu->addAction(p_checkableAction);
        connect(p_checkBox, &QCheckBox::toggled, this, &Inventory::setVisibilityFromCheckBox);
    }
    QPoint *p_position = new QPoint(pos.x(), pos.y());
    p_position->ry() -= 30; // Correct off set popup window
    p_popUpMenu->popup(ui->tableView->viewport()->mapToGlobal(*p_position));
};

void Inventory::setVisibilityFromCheckBox() {
    auto *p_checkBox = qobject_cast<QCheckBox *>(sender());
    Field index = static_cast<Inventory::Field>(p_checkBox->objectName().toInt());
    setVisibility(index, p_checkBox->isChecked());
}

bool Inventory::getVisibilityColumn(Field field) {
    return columnVisibility.value(field);
}

void Inventory::setVisibility(Field field) {
    bool visible = getVisibilityColumn(field);
    setVisibility(field, visible);
}

void Inventory::setVisibility(Field field, bool visible) {
    if(visible) {
        ui->tableView->showColumn(field);
        columnVisibility[field] = true;
    } else {
        ui->tableView->hideColumn(field);
        columnVisibility[field] = false;
    }
}

QString Inventory::getHeader(Field field) {
    switch (field) {
    case Field::InventoryID:
        return QStringLiteral("ID");
    case Field::ItemNo:
        return QStringLiteral("Item Number");
    case Field::ItemName:
        return QStringLiteral("Item Name");
    case Field::ItemType:
        return QStringLiteral("Item Type");
    case Field::CategoryID:
        return QStringLiteral("Category ID");
    case Field::CategoryName:
        return QStringLiteral("Category");
    case Field::ColorId:
        return QStringLiteral("Color ID");
    case Field::ColorName:
        return QStringLiteral("Color");
    case Field::Quantity:
        return QStringLiteral("Quantity");
    case Field::NewOrUsed:
        return QStringLiteral("Condition");
    case Field::Completeness:
        return QStringLiteral("Completeness");
    case Field::UnitPrice:
        return QStringLiteral("Unit Price");
    case Field::UnitPriceFinal:
        return QStringLiteral("Unit Price Final");
    case Field::DispUnitPrice:
        return QStringLiteral("Unit Price Buyer");
    case Field::DispUnitPriceFinal:
        return QStringLiteral("Unit Price Buyer Final");
    case Field::CurrencyCode:
        return QStringLiteral("Currency");
    case Field::DispCurrencyCode:
        return QStringLiteral("Currency User");
    case Field::Remarks:
        return QStringLiteral("Remarks");
    case Field::Description:
        return QStringLiteral("Description");
    case Field::Weight:
        return QStringLiteral("Weight");
    case Field::BatchNumber:
        return QStringLiteral("Batch");
    }
}
