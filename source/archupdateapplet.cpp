#include "archupdateapplet.h"
#include <QMouseEvent>
#ifdef QT_DEBUG
#include <QDebug>
#endif

#define WIDTH 350
#define WIDTH_MIN 125
#define HEIGHT 500

ArchUpdateApplet::ArchUpdateApplet(const ArchUpdateData* data, QWidget *parent) :
    QWidget(parent), m_data(data),
    packlist(new UpdateList(this)),
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

    connect(updateButton, SIGNAL(released()), this, SIGNAL(updateSys()));
}

void ArchUpdateApplet::refreshList() {
    packlist->clear();
    if(m_data->newcount() == 0) {
        // TODO set it grey
        QListWidgetItem* nopack = new QListWidgetItem(tr("  ..(None)..  "), packlist);
        nopack->setTextAlignment(Qt::AlignHCenter);
        packlist->addItem(nopack);
        packlist->setMouseTracking(false);
        packlist->setStyleSheet("");
    }
    else {
        packlist->addItems(m_data->newpackList());
        for (int i=0; i<packlist->count(); i++) {
            QListWidgetItem *item = packlist->item(i);
            item->setToolTip(QString("%1 → %2").arg(m_data->currentV(i)).arg(
                                 m_data->newV(i)));
        }
        #ifdef QT_DEBUG
        qDebug()<<"-->Arch Update Applet packlist count: "<<packlist->count();
        #endif
        packlist->setMouseTracking(true);
        packlist->setStyleSheet("QListWidget::item:hover {background-color:rgba(255, 255, 255, .2);}");
    }
    int w = packlist->sizeHintForColumn(0) + 5;
    if(w < WIDTH_MIN)
        w = WIDTH_MIN;
    else if(w > WIDTH)
        w = WIDTH;
    packlist->setFixedWidth(w);
    int h = packlist->sizeHintForRow(0) * packlist->count() + 4;
    packlist->setFixedHeight(h < HEIGHT ? h : HEIGHT);
    update();
}

void UpdateList::startTip() {
    showTip = true;
    popTip(this->mapFromGlobal(QCursor::pos()));
}

void UpdateList::enterEvent(QEvent *event) {
    if (!(mouseInTimer.isActive() || showTip ))
        mouseInTimer.start();
    QListWidget::enterEvent(event);
}

void UpdateList::leaveEvent(QEvent *event) {
    mouseInTimer.stop();
    showTip = false;
    QListWidget::leaveEvent(event);
}

void UpdateList::mouseMoveEvent (QMouseEvent *event) {
    if (showTip) {
        popTip(event->pos());
    }
    QListWidget::mouseMoveEvent(event);
}

void UpdateList::popTip(const QPoint &p) {
    QListWidgetItem *theItem = this->itemAt(p);
    if (theItem != nullptr)
        QToolTip::showText(this->mapToGlobal(p),
                           theItem->toolTip(),
                           this, this->rect());
}

void UpdateList::clear() {
    if (showTip) {
        // Restart a timer
        showTip = false;
        mouseInTimer.start();
    }
    QListWidget::clear();
}
