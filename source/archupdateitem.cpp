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
    if(displayMode == Dock::Efficient)
        iconname += "_white";
#ifdef QT_DEBUG
    qDebug()<<"refreshIcon: "<<iconname;
#endif
    m_iconPixmap = QIcon(iconname).pixmap(static_cast<int>(iconSize * ratio));
    m_iconPixmap.setDevicePixelRatio(ratio);
    update();
}
