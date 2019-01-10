#ifndef ARCHUPDATEITEM_H
#define ARCHUPDATEITEM_H

#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>
#include "archupdatedata.h"

class ArchUpdateItem : public QWidget {
    Q_OBJECT
public:
    explicit ArchUpdateItem(const ArchUpdateData* data, QWidget *parent=nullptr);

protected:
    QSize sizeHint() const {return QSize(26, 26); }
    void resizeEvent(QResizeEvent *e){
        QWidget::resizeEvent(e);
        refreshIcon();
    }
    void paintEvent(QPaintEvent *e);

public slots:
    void refreshIcon();

private:
    const ArchUpdateData* m_data;
    QPixmap m_iconPixmap;
};


#endif // ARCHUPDATEITEM_H
