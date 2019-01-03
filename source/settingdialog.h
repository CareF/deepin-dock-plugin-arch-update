#ifndef ARCHUPDATESETTINGDIAG_H
#define ARCHUPDATESETTINGDIAG_H
#include <QDialog>
#include <QValidator>
#include <QList>
#include <QLineEdit>
#include <QPointer>

struct settingItem {
    const QString name;
    QString currentValue;
    const QString hint; // TODO for what's this
    const QString defaultValue;
    const QValidator *validator;
};

class SettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingDialog(QList<settingItem> &desc, QWidget *parent=nullptr);
    virtual ~SettingDialog() override;

public slots:
    virtual void accept() override;
    virtual void reset();

private:
    QList<settingItem> &config;
    QPointer<QLineEdit> *inputs;
};

#endif // ARCHUPDATESETTINGDIAG_H
