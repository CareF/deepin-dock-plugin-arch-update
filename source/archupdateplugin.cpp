#include "archupdateplugin.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QIcon>
#include <QProcess>
#ifdef QT_DEBUG
#include <QDebug>
#endif

// itemID or menuID for Context Menu.. Why can't I use enum and switch..
#define CHECKUPDATE "chk"
#define SETTINGS "set"
#define ABOUT "about"
// keys for settings from .config/deepin/dde-dock.conf, set and get through m_proxyInter
#define CHECK_CMD_KEY "check_command"
#define PACMAN_DIR_KEY "pacman_dir"
#define UPDATE_CMD_KEY "update_cmd"

ArchUpdatePlugin::ArchUpdatePlugin(QObject *parent):
    QObject(parent), m_items(nullptr), m_popups(nullptr), m_tips(nullptr),
    m_data(nullptr), pacman_dir(), update_cmd(), pacmanWatcher(this) {
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

    m_data = new ArchUpdateData(
                m_proxyInter->getValue(this, CHECK_CMD_KEY,
                    DEFAULT_CHK_UPDATE).toString());
    m_data->moveToThread(&m_updateThread);
    connect(this, &ArchUpdatePlugin::checkUpdate,
            m_data, &ArchUpdateData::check);
    connect(m_data, &ArchUpdateData::finished,
            this, &ArchUpdatePlugin::refreshTips);
    m_updateThread.start();

    m_items = new ArchUpdateItem(m_data);
    // m_items->setWindowIcon(QIcon(":/icons/arch-lit-symbolic.svg")); // used for about window
    m_items->setWindowIcon(QIcon(":/lit")); // used for about window
    connect(m_items, &ArchUpdateItem::requestContextMenu, [this] {
            m_proxyInter->requestContextMenu(this, ARCH_KEY); });
    connect(m_data, &ArchUpdateData::finished,
            m_items, &ArchUpdateItem::refreshIcon);
    connect(this, &ArchUpdatePlugin::checkUpdate,
            m_items, &ArchUpdateItem::refreshIcon);

    m_popups = new ArchUpdateApplet(m_data);
    connect(m_data, &ArchUpdateData::finished,
            m_popups, &ArchUpdateApplet::refreshList);
    connect(m_popups, &ArchUpdateApplet::update,
            this, &ArchUpdatePlugin::updatesystem);

    m_tips = new QLabel();
    m_tips->setObjectName("arch_update_tips");
    m_tips->setStyleSheet("color:white; padding:0px 3px;");

    pacman_dir = m_proxyInter->getValue(this, PACMAN_DIR_KEY,
                        "/var/lib/pacman/local").toString();
    update_cmd = m_proxyInter->getValue(this, UPDATE_CMD_KEY,
                        DEFAULT_UPDATE).toString();
    pacmanWatcher.addPath(pacman_dir);
    connect(&pacmanWatcher, &QFileSystemWatcher::directoryChanged,
            this, &ArchUpdatePlugin::fileChanged);
    if(!pluginIsDisable()) {
        this->m_proxyInter->itemAdded(this, ARCH_KEY);
        emit checkUpdate();
    }
}

void ArchUpdatePlugin::refreshTips() {
#ifdef QT_DEBUG
    qDebug()<<"----- Refresh Tips!!";
#endif
    QString updates;
    if (m_data->error_code() != 0) {
        updates = tr("Error\n");
    }
    else if(m_data->newcount() != 0) {
        updates = tr("%1 new packages\n").arg(m_data->newcount());
    }
    else{
        updates = tr("Up to date :)\n");
    }
    m_tips->setText(updates+tr("Last check: %2").arg(
                m_data->lastcheck.toString("M/d H:m")));
}

void ArchUpdatePlugin::fileChanged() {
    // TODO: wait for some time when stop tracking the signal,
    // than emmit checkUpdate
    emit checkUpdate();
}

void ArchUpdatePlugin::updatesystem() {
    // TODO: call terminal to updat system
    QProcess *updateprocess = new QProcess(this);
    connect(updateprocess, SIGNAL(finished(int)), updateprocess, SLOT(deleteLater));
    updateprocess->start(update_cmd);
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
            execSettingDiag();
        }
        else if (menuID == ABOUT) {
            QMessageBox::about(m_items, tr("Arch Update: About"),
                              tr("Deepin Dock Plugin: Arch Update Indicator.\n"
                                 "License: GPLv3.0\n"
                                 "Author: CareF <me@mail.caref.xyz>\n"
                                 "Source: https://github.com/CareF/deepin-dock-plugin-arch-update"));
            // TODO: clickable url?
        }
    }
}

void ArchUpdatePlugin::execSettingDiag() {
    // TODO: setting for checkupdate command, pacman dir, time interval, update command
}

int ArchUpdatePlugin::itemSortKey(const QString &itemKey) {
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(displayMode());
    return m_proxyInter->getValue(this, key, 
            displayMode() == Dock::DisplayMode::Fashion ? 2 : 3).toInt();
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

