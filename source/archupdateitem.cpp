#include "archupdateitem.h"
#include "dde-dock/constants.h" // provide Dock::DisplayMode
#include <QApplication> // provide qApp
//#include <QSvgRenderer>
#include <QPainter>
#include <QIcon>
#ifdef QT_DEBUG
#include <QDebug>
#endif

ArchUpdateItem::ArchUpdateItem(const ArchUpdateData* data, QWidget *parent) :
    QWidget(parent), m_data(data) {

}

void ArchUpdateItem::mousePressEvent(QMouseEvent *e) {
    if (e->button() != Qt::RightButton)
        return QWidget::mousePressEvent(e);

    const QPoint p(e->pos() - rect().center());
    if (p.manhattanLength() < std::min(width(), height()) * 0.8 * 0.5)
    {
        e->accept();
        emit requestContextMenu();
        return;
    }

    return QWidget::mousePressEvent(e);
}

void ArchUpdateItem::paintEvent(QPaintEvent *e) {
    QWidget::paintEvent(e);
    QPainter painter(this);
    const QRectF &rf = QRectF(rect());
    const QRectF &rfp = QRectF(m_iconPixmap.rect());
    painter.drawPixmap(rf.center() - rfp.center() / m_iconPixmap.devicePixelRatioF(), m_iconPixmap);
}

void ArchUpdateItem::refreshIcon() {
     const Dock::DisplayMode displayMode = qApp->property(PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
    // TODO: cache these pixmap
    // const Dock::DisplayMode displayMode = Dock::DisplayMode::Efficient;
    const auto ratio = qApp->devicePixelRatio();
    const int iconSize = (displayMode == Dock::Fashion ?
                              static_cast<int>(std::min(width(), height()) * 0.8) : 16);
    QString state;
    if (m_data->is_checking()) {
        state = "unkown";
    }
    else if (m_data->error_code()) {
        state = "error";
    }
    else if (m_data->newcount() > 0) {
        state = "updates";
    }
    else {
        state = "uptodate";
    }
    // QString iconname = QString(":icons/arch-%1-symbolic.svg").arg(state);
    QString iconname = QString(":/%1").arg(state);
#ifdef QT_DEBUG
    qDebug()<<"refreshIcon: "<<iconname;
#endif
    m_iconPixmap = QIcon(iconname).pixmap(static_cast<int>(iconSize*ratio));
    update();
}
