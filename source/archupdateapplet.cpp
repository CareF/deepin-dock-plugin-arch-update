#include "archupdateapplet.h"
#ifdef QT_DEBUG
#include <QDebug>
#endif

#define WIDTH 280
#define HEIGHT 800

ArchUpdateApplet::ArchUpdateApplet(const ArchUpdateData* data, QWidget *parent) :
    QWidget(parent), m_data(data),
    packlist(new QListWidget(this)),
    updateButton(new QPushButton(tr("Update"), this))
{
    packlist->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    packlist->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    packlist->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    packlist->setMaximumWidth(WIDTH);
    packlist->setMaximumHeight(HEIGHT);
    packlist->setSelectionMode(QAbstractItemView::NoSelection);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(packlist);
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    // line->setFrameShadow(QFrame::Sunken);
    vLayout->addWidget(line);
    vLayout->addWidget(updateButton);
    setLayout(vLayout);

    connect(updateButton, SIGNAL(released()), this, SIGNAL(update()));
}

void ArchUpdateApplet::refreshList() {
    packlist->clear();
    if(m_data->newcount() == 0) {
        QListWidgetItem* nopack = new QListWidgetItem(tr("..(None).."), packlist);
        nopack->setTextAlignment(Qt::AlignHCenter);
        packlist->addItem(nopack);
    }
    else {
        // TODO: show version when hover
        packlist->addItems(m_data->newpackList());
        int w = packlist->sizeHintForColumn(0) + 5;
        packlist->setFixedWidth(w < WIDTH ? w : WIDTH);
        int h = packlist->sizeHintForRow(0) * packlist->count() + 4;
        packlist->setFixedHeight(h < HEIGHT ? h : HEIGHT);
        #ifdef QT_DEBUG
        qDebug()<<"-->Arch Update Applet packlist count: "<<packlist->count();
        #endif
    }
}
