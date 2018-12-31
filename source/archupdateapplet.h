#ifndef ARCHUPDATEAPPLET_H
#define ARCHUPDATEAPPLET_H

#include <QWidget>
#include <QPointer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include "archupdatedata.h"

class ArchUpdateApplet : public QWidget {
    Q_OBJECT
public:
    explicit ArchUpdateApplet(const ArchUpdateData* data, QWidget *parent = nullptr);

signals:
    void update();

public slots:
    void refreshList();

private:
    const ArchUpdateData* m_data;
    QPointer<QListWidget> packlist;
    QPointer<QPushButton> updateButton;
};

#endif // ARCHUPDATEAPPLET_H
