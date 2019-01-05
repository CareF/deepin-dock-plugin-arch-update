#include <QBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QDialog>
#include "settingdialog.h"
#ifdef QT_DEBUG
#include <QDebug>
#endif
#define WIDTH 280

DWIDGET_USE_NAMESPACE

SettingDialog::SettingDialog(QVector<settingItem> &desc, QWidget *parent):
    DAbstractDialog (parent), config(desc) {
    // setWindowTitle(tr("Arch Update Settings"));
    QVBoxLayout *vLayout = new QVBoxLayout();
    QGridLayout *itemLayout = new QGridLayout();
    inputs = new QPointer<QLineEdit> [config.size()];
    for (int n=0; n<config.size(); n++) {
        itemLayout->addWidget(new QLabel(config[n].name, this), n, 0);
        inputs[n] = new QLineEdit(config[n].currentValue, this);
        inputs[n]->setMinimumWidth(WIDTH);
        inputs[n]->setValidator(config[n].validator);
        inputs[n]->setWhatsThis(config[n].hint);
        itemLayout->addWidget(inputs[n], n, 1);
    }
    vLayout->addLayout(itemLayout);

    QDialogButtonBox *buttons = new QDialogButtonBox(
                QDialogButtonBox::Ok|
                QDialogButtonBox::Cancel|
                QDialogButtonBox::Reset, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons->button(QDialogButtonBox::Reset), &QPushButton::released,
            this, &SettingDialog::reset);
    vLayout->addWidget(buttons);

    setLayout(vLayout);
}

SettingDialog::~SettingDialog() {
    delete[] inputs;
}

void SettingDialog::accept() {
    for (int n=0; n<config.size(); n++) {
        config[n].currentValue = inputs[n]->text();
    }
    QDialog::accept();
}

void SettingDialog::reset() {
    for (int n=0; n<config.size(); n++) {
        inputs[n]->setText(config[n].defaultValue);
    }
}
