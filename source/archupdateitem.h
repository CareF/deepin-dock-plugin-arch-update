#ifndef ARCHUPDATEITEM_H
#define ARCHUPDATEITEM_H

#include <QWidget>
#include "archupdatedata.h"

class ArchUpdateItem : public QWidget {
    Q_OBJECT
public:
    explicit ArchUpdateItem(const ArchUpdateData* data, QWidget *parent=nullptr);
    void refreshIcon();

signals:
    void requestContextMenu() const;


public slots:

private:
    const ArchUpdateData* m_data;
};


#endif // ARCHUPDATEITEM_H
