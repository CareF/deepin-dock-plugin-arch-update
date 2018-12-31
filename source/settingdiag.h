#ifndef ARCHUPDATESETTINGDIAG_H
#define ARCHUPDATESETTINGDIAG_H
#include <QDialog>
#include <QValidator>
#include <QList>

struct settingItem {
    const QString &name;
    QString currentValue;
    const QString &hint; // TODO for what's this
    const QString &defaultValue;
    const QValidator *validator;
};

class SettingDiag : public QDialog
{
    Q_OBJECT
public:
    SettingDiag(QList<settingItem> &in, QWidget *parent=nullptr);

private:
    QList<settingItem> &config;
};

#endif // ARCHUPDATESETTINGDIAG_H
