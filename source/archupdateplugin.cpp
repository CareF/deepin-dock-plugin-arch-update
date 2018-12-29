#include "archupdate.h"

ArchUpdatePlugin::ArchUpdatePlugin(QObject *parent):
    QObject(parent), m_data(new ArchUpdateData), 
    m_items(nullptr), m_popups(nullptr), m_tips(nullptr) {
}

ArchUpdatePlugin::~ArchUpdatePlugin() {
    delete m_data;
}

const QString ArchUpdatePlugin::pluginName() const {
    return "arch_update";
}

const QString ArchUpdatePlugin::pluginDisplayName() const {
    return tr("Arch Update");
}

void ArchUpdatePlugin::init(PluginProxyInterface *proxyInter) {
    this->m_proxyInter = proxyInter;
    m_items = new ArchUpdateItem(this, m_data);
    connect(m_items, &ArchUpdateItem::requestContextMenu, [this] {
            m_proxyInter->requestContextMenu(this, ARCH_KEY); });
    m_popups = new ArchUpdateApplet(this, m_data);
    m_tips = new QLabel(this);
    m_tips->setObjectName("arch_update_tips");
    m_tips->setStyleSheet("color:white; padding:0px 3px;");
    if(!pluginIsDisable()) {
        this->m_proxyInter->itemAdded(this, ARCH_KEY);
        checkUpdate();
    }
}

void ArchUpdatePlugin::checkUpdate() {
    // TODO: Run update results and store them in m_data
    m_tips->setText(tr("%d new packages\nLast check: %s").arg(
                m_data->newpacks).arg(
                m_data->lastcheck.toString("M/d H:m")));
}

bool ArchUpdatePlugin::pluginStateSwitched() {
    m_proxyInter->saveValue(this, STATE_KEY, pluginIsDisable());
    if (pluginIsDisable())
        m_proxyInter->itemRemoved(this, ARCH_KEY);
    else
        m_proxyInter->itemAdded(this, ARCH_KEY);
}

bool ArchUpdatePlugin::pluginIsDisable() {
    return !m_proxyInter->getValue(this, STATE_KEY, true).toBool();
}

QWidget *ArchUpdatePlugin::itemWidget(const QString &itemKey) {
    if (itemKey == ARCH_KEY) {
        return m_items;
    }
    return nullptr;
}

QWidget *ArchUpdatePlugin::itemTipsWidget(const QString &itemKey) {
    if (itemKey == ARCH_KEY) {
        return m_tips();
    }
    return nullptr;
}

QWidget *ArchUpdatePlugin::itemPopupApplet(const QString &itemKey) {
    if (itemKey == ARCH_KEY) {
        return m_popups();
    }
    return nullptr;
}

const QString itemContextMenu(const QString &itemKey) {
    if (itemKey == ARCH_KEY) {
        // TODO:
    }
    return QString();
}

void ArchUpdatePlugin::invokedMenuItem(const QString &itemKey, 
        const QString &menuID, const bool checked) {
    if (itemKey == ARCH_KEY) {
        // TODO:
    }
}

int ArchUpdatePlugin::itemSortKey(const QString &itemKey) {
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(displayMode());
    return m_proxyInter->getValue(this, key, 
            displayMode() == Dock::DisplayMode::Fashion ? 1 : 1).toInt();
}

void ArchUpdatePlugin::setSortKey(const QString &itemKey, const int order) {
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(displayMode());
    m_proxyInter->saveValue(this, key, order);
}

void ArchUpdatePlugin::refreshIcon(const QString &itemKey) {
    if (itemKey == ARCH_KEY) {
        m_items->refreshIcon();
    }
}

