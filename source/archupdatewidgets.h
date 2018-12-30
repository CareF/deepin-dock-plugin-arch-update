#ifndef ARCHUPDATEWIDGETS_H
#define ARCHUPDATEWIDGETS_H

#include "archupdatedata.h"
#include <QWidget>

class ArchUpdateItem: public QWidget {
    Q_OBJECT

public:
    explicit ArchUpdateItem(const ArchUpdateData* data, QWidget *parent=nullptr);
    void refreshIcon();

signals:
    void requestContextMenu() const;

private:
    const ArchUpdateData* m_data;
};

class ArchUpdateApplet: public QWidget {
    Q_OBJECT

public:
    explicit ArchUpdateApplet(const ArchUpdateData* data, QWidget *parent=nullptr);

private:
    const ArchUpdateData* m_data;
};

#endif // ARCHUPDATEWIDGETS_H
