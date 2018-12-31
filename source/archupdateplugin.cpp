#include "archupdateplugin.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QIcon>
#include <QProcess>
#ifdef QT_DEBUG
#include <QDebug>
#endif
#include "settingdiag.h"
#define MINUTE 60000 //1min in milisecond

ArchUpdatePlugin::ArchUpdatePlugin(QObject *parent):
    QObject(parent),
    m_items(nullptr), m_popups(nullptr), m_tips(nullptr),
    m_data(nullptr), pacman_dir(), update_cmd(),
    pacmanWatcher(this), watcherTimer(this), regularTimer(this) {
    watcherTimer.setSingleShot(true);
    connect(&watcherTimer, &QTimer::timeout,
            this, &ArchUpdatePlugin::checkUpdate);
    connect(&regularTimer, &QTimer::timeout,
            this, &ArchUpdatePlugin::checkUpdate);
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
                        DEFAULT_PACMAN_DIR).toString();
    update_cmd = m_proxyInter->getValue(this, UPDATE_CMD_KEY,
                        DEFAULT_UPDATE).toString();
    // pacmanWatcher.removePaths(pacmanWatcher.files());
    // ? Is init called every time the plugin is loaded and unloaded
    // without or with distroy and recreate it?

    if(!pluginIsDisable()) {
        this->m_proxyInter->itemAdded(this, ARCH_KEY);
        pacmanWatcher.addPath(pacman_dir);
        connect(&pacmanWatcher, &QFileSystemWatcher::directoryChanged,
                this, &ArchUpdatePlugin::fileChanged);
        regularTimer.setInterval(MINUTE * m_proxyInter->getValue(
                                     this, CHK_INTERVAL_KEY, 30).toInt());
        emit checkUpdate();
    }  else {
        disconnect(&pacmanWatcher, &QFileSystemWatcher::directoryChanged,
                   this, &ArchUpdatePlugin::fileChanged);
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
    // wait for 1min when stop tracking the signal,
    // than emmit checkUpdate
    if (watcherTimer.isActive())
        return;
    watcherTimer.start(MINUTE/2); // 30s
}

void ArchUpdatePlugin::updatesystem() {
    // call terminal to updat system
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

const QIntValidator ArchUpdatePlugin::TIMEINMIN(1, 99999);
void ArchUpdatePlugin::execSettingDiag() {
    // TODO: setting for checkupdate command, pacman dir, time interval, update command
    QList<settingItem> config = {
        {tr("Checkupdate"), m_data->check_cmd,
         tr("The shell command to check updates (Default `checkupdates` provided by pacman-contrib)"),
         DEFAULT_CHK_UPDATE, nullptr},
        {tr("Pacman local directory path"), pacman_dir,
         tr("The plugin watch this path to detect when new packages are installed"),
         DEFAULT_PACMAN_DIR, nullptr},
        {tr("Update shell cmd"), update_cmd, "", DEFAULT_UPDATE, nullptr},
        {tr("Time Interval (min)"), QString(regularTimer.interval()/MINUTE),
         tr("Interval between updates check (minutes)"),
         DEFAULT_CHK_UPDATE, &TIMEINMIN}
    };
    SettingDiag settingDiag(config);
    switch (settingDiag.exec()) {
    case QDialog::Rejected:
        return;
    case QDialog::Accepted:
        break;
    }
    m_proxyInter->saveValue(this, CHECK_CMD_KEY, config[0].currentValue);
    m_proxyInter->saveValue(this, PACMAN_DIR_KEY, config[1].currentValue);
    m_proxyInter->saveValue(this, UPDATE_CMD_KEY, config[2].currentValue);
    m_proxyInter->saveValue(this, CHK_INTERVAL_KEY, config[3].currentValue.toInt());
    reloadSetting();
}

void ArchUpdatePlugin::reloadSetting() {
    m_data->check_cmd = m_proxyInter->getValue(
                this, CHECK_CMD_KEY).toString();
    pacman_dir = m_proxyInter->getValue(this, PACMAN_DIR_KEY).toString();
    update_cmd = m_proxyInter->getValue(this, UPDATE_CMD_KEY).toString();
    pacmanWatcher.removePaths(pacmanWatcher.files());
    if(!pluginIsDisable()) {
        pacmanWatcher.addPath(pacman_dir);
        regularTimer.setInterval(
                    MINUTE * m_proxyInter->getValue(
                        this, CHK_INTERVAL_KEY).toInt());
    }
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

