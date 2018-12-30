#include "archupdateplugin.h"
#include <QJsonDocument>
#include <QMessageBox>

// itemID or menuID for Context Menu.. Why can't I use enum and switch..
#define CHECKUPDATE "chk"
#define SETTINGS "set"
#define ABOUT "about"
// keys for settings from .config/deepin/dde-dock.conf, set and get through m_proxyInter
#define CHECK_CMD_KEY "check_command"
#define PACMAN_DIR_KEY "pacman_dir"

ArchUpdatePlugin::ArchUpdatePlugin(QObject *parent):
    QObject(parent), m_items(nullptr), m_popups(nullptr),
    m_tips(nullptr), m_data(nullptr), pacman_dir() {
}

ArchUpdatePlugin::~ArchUpdatePlugin() {
    delete m_items;
    delete m_popups;
    delete m_tips;
    delete m_data;
    m_updateThread.quit();
    m_updateThread.wait();
}

const QString ArchUpdatePlugin::pluginName() const {
    return "arch_update";
}

const QString ArchUpdatePlugin::pluginDisplayName() const {
    return tr("Arch Update");
}

void ArchUpdatePlugin::init(PluginProxyInterface *proxyInter) {
    this->m_proxyInter = proxyInter;
    m_items = new ArchUpdateItem(m_data);
    connect(m_items, &ArchUpdateItem::requestContextMenu, [this] {
            m_proxyInter->requestContextMenu(this, ARCH_KEY); });

    m_popups = new ArchUpdateApplet(m_data);

    m_tips = new QLabel();
    m_tips->setObjectName("arch_update_tips");
    m_tips->setStyleSheet("color:white; padding:0px 3px;");

    m_data = new ArchUpdateData(
                m_proxyInter->getValue(this, CHECK_CMD_KEY,
                    "/usr/bin/checkupdates").toString());
    m_data->moveToThread(&m_updateThread);
    connect(this, &ArchUpdatePlugin::checkUpdate,
            m_data, &ArchUpdateData::check);

    pacman_dir = m_proxyInter->getValue(this, PACMAN_DIR_KEY,
                        "/var/lib/pacman/local").toString();
    if(!pluginIsDisable()) {
        this->m_proxyInter->itemAdded(this, ARCH_KEY);
        emit checkUpdate();
    }
}

void ArchUpdatePlugin::updateTips() {
    m_tips->setText(tr("%d new packages\nLast check: %s").arg(
                m_data->newcount()).arg(
                m_data->lastcheck.toString("M/d H:m")));
}

void ArchUpdatePlugin::pluginStateSwitched() {
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
        return m_tips;
    }
    return nullptr;
}

QWidget *ArchUpdatePlugin::itemPopupApplet(const QString &itemKey) {
    if (itemKey == ARCH_KEY) {
        return m_popups;
    }
    return nullptr;
}

const QString ArchUpdatePlugin::itemContextMenu(const QString &itemKey) {
    if (itemKey == ARCH_KEY) {
        QList<QVariant> items;
        items.reserve(3);

        QMap<QString, QVariant> check;
        check["itemId"] = CHECKUPDATE;
        if (m_data->is_checking()) {
            check["itemText"] = tr("Checking...");
            check["isActive"] = false;
        }
        else {
            check["itemText"] = tr("Check now");
            check["isActive"] = true;
        }
        items.push_back(check);

        QMap<QString, QVariant> settings;
        settings["itemId"] = SETTINGS;
        settings["itemText"] = tr("Settings");
        settings["isActive"] = true;
        items.push_back(settings);

        QMap<QString, QVariant> about;
        about["itemId"] = ABOUT;
        about["itemText"] = tr("About");
        about["isActive"] = true;
        items.push_back(about);

        QMap<QString, QVariant> menu;
        menu["items"] = items;
        menu["checkableMenu"] = false;
        menu["singleCheck"] = false;

        return QJsonDocument::fromVariant(menu).toJson();
    }
    return QString();
}

void ArchUpdatePlugin::invokedMenuItem(const QString &itemKey, 
        const QString &menuID, const bool checked) {
    Q_UNUSED(checked)
    if (itemKey == ARCH_KEY) {
        if (menuID == CHECKUPDATE) {
            emit checkUpdate();
        }
        else if (menuID == SETTINGS) {
            // TODO
        }
        else if (menuID == ABOUT) {
            QMessageBox::about(nullptr, tr("Arch Update: About"),
                              tr("Deepin Dock Plugin: Arch Update Indicator.\n"
                              "Author: CareF\n"
                              "Source: <a href='https://github.com/CareF/deepin-dock-plugin-arch-update'>GitHub</a>"));
        }
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

