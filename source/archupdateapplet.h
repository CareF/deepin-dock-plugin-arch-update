#ifndef ARCHUPDATEAPPLET_H
#define ARCHUPDATEAPPLET_H

#include <QWidget>
#include "archupdatedata.h"

class ArchUpdateApplet : public QWidget {
    Q_OBJECT
public:
    explicit ArchUpdateApplet(const ArchUpdateData* data, QWidget *parent = nullptr);

signals:

public slots:

private:
    const ArchUpdateData* m_data;
};

#endif // ARCHUPDATEAPPLET_H
