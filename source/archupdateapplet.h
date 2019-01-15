#ifndef ARCHUPDATEAPPLET_H
#define ARCHUPDATEAPPLET_H

#include <QWidget>
#include <QPointer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <QToolTip>
#include "archupdatedata.h"

class UpdateList: public QListWidget {
    Q_OBJECT
public:
    explicit UpdateList(QWidget *parent=nullptr):
        QListWidget(parent), showTip(false){
        mouseInTimer.setSingleShot(true);
        mouseInTimer.setInterval(500); // 0.5s
        mouseInTimer.callOnTimeout(this, &UpdateList::startTip);
    }

protected:
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseMoveEvent (QMouseEvent *event) override;

    void popTip(const QPoint &p);

protected slots:
    void startTip();

private:
    QTimer mouseInTimer;
    bool showTip;
};

class ArchUpdateApplet : public QWidget {
    Q_OBJECT
public:
    explicit ArchUpdateApplet(const ArchUpdateData* data, QWidget *parent = nullptr);

signals:
    void updateSys();

public slots:
    void refreshList();

private:
    const ArchUpdateData* m_data;
    QPointer<UpdateList> packlist;
    QPointer<QPushButton> updateButton;
};

#endif // ARCHUPDATEAPPLET_H
