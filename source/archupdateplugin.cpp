#include "archupdateplugin.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QIcon>
#include <QProcess>
#include <QApplication>
#include <QTranslator>
#include <QException>
#include <DAboutDialog>
#ifdef QT_DEBUG
#include <QDebug>
#endif
#define MINUTE 60000 //1min in milisecond

QTranslator ArchUpdatePlugin::qtTranslator;
QTranslator *ArchUpdatePlugin::loadTranslator(const QLocale &locale, QObject *parent) {
    // This is a dirty trick to load translator before init of config
    QTranslator* ts = new QTranslator(parent);
    // QString lang = QLocale::system().name();
    // ts->load(QString(":/i18n/archupdate-%1.qm").arg(lang);
    // ts->load(":/i18n/archupdate-zh_CN.qm");
#ifdef QT_DEBUG
    qDebug() << "---- Arch Update Plugin: Load Language: " << locale.name();
#endif
    ts->load(locale, "archupdate", "-", ":/i18n", ".qm");
    ArchUpdatePlugin::qtTranslator.load(
                locale, "qtbase", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::instance()->installTranslator(ts);
    // QApplication::instance()->installTranslator(&ArchUpdatePlugin::qtTranslator);
    return ts;
}

ArchUpdatePlugin::ArchUpdatePlugin(QObject *parent):
    QObject(parent), translator(loadTranslator(QLocale::system(), this)),
    m_items(nullptr), m_popups(nullptr), m_tips(nullptr),
    m_data(nullptr), pacman_dir(), update_cmd(),
    pacmanWatcher(this), watcherTimer(this), regularTimer(this),
    config({
                {tr("Checkupdate"), DEFAULT_CHK_UPDATE,
                 tr("The shell command to check updates (Default `checkupdates` provided by pacman-contrib)"),
                 DEFAULT_CHK_UPDATE, nullptr},
                {tr("Pacman local path"), DEFAULT_PACMAN_DIR,
                 tr("The plugin watch this path to detect when new packages are installed"),
                 DEFAULT_PACMAN_DIR, nullptr},
                {tr("Update shell cmd"), DEFAULT_UPDATE, "", DEFAULT_UPDATE, nullptr},
                {tr("Time Interval (min)"), QString::number(DEFAULT_INTERVAL),
                 tr("Interval between updates check (minutes)"),
                 QString::number(DEFAULT_INTERVAL), &TIMEINMIN}
            })
{
    watcherTimer.setSingleShot(true);
    connect(&watcherTimer, &QTimer::timeout,
            this, &ArchUpdatePlugin::checkUpdate);
    connect(&regularTimer, &QTimer::timeout,
            this, &ArchUpdatePlugin::checkUpdate);
    settingDialog = new SettingDialog(config);
    settingDialog->setWindowModality(Qt::NonModal);
    connect(settingDialog, &SettingDialog::accepted, this, &ArchUpdatePlugin::reloadSetting);
}

ArchUpdatePlugin::~ArchUpdatePlugin() {
    delete m_items;
    delete m_popups;
    delete m_tips;
    delete m_data;
    delete settingDialog;
    m_updateThread.quit();
    m_updateThread.wait();
    QApplication::instance()->removeTranslator(translator);
    QApplication::instance()->removeTranslator(&ArchUpdatePlugin::qtTranslator);
    delete translator;
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
    connect(m_data, &ArchUpdateData::finished,
            m_items, &ArchUpdateItem::refreshIcon);
    connect(this, &ArchUpdatePlugin::checkUpdate,
            m_items, &ArchUpdateItem::refreshIcon);

    m_popups = new ArchUpdateApplet(m_data);
    connect(m_data, &ArchUpdateData::finished,
            m_popups, &ArchUpdateApplet::refreshList);
    connect(m_popups, &ArchUpdateApplet::updateSys,
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
        emit checkUpdate();
        regularTimer.start(MINUTE * m_proxyInter->getValue(
                               this, CHK_INTERVAL_KEY, DEFAULT_INTERVAL).toInt());
    }
}

void ArchUpdatePlugin::refreshTips() {
#ifdef QT_DEBUG
    qDebug()<<"----- Refresh Tips!!";
#endif
    QString updates;
    if (m_data->error_code() != 0) {
        updates = tr("Error: %1\n").arg(m_data->error_code());
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
    // wait for 5s than emmit checkUpdate
    m_data->ischecking = true;
    watcherTimer.start(MINUTE/12); // 5s
#ifdef QT_DEBUG
    qDebug()<<"----ArchUpdate: start watcherTimer";
#endif
    m_items->refreshIcon();
}
void ArchUpdatePlugin::updatesystem() {
    // call terminal to updat system
    QProcess *updateprocess = new QProcess(this);
    connect(updateprocess, SIGNAL(finished(int)), updateprocess, SLOT(deleteLater));
    updateprocess->start(update_cmd);
    m_proxyInter->requestSetAppletVisible(this, ARCH_KEY, false);
}

void ArchUpdatePlugin::pluginStateSwitched() {
    m_proxyInter->saveValue(this, STATE_KEY, pluginIsDisable());
    if (pluginIsDisable()) {
        regularTimer.stop();
        pacmanWatcher.removePaths(pacmanWatcher.files());
        m_proxyInter->itemRemoved(this, ARCH_KEY);
    }
    else {
        regularTimer.start();
        pacmanWatcher.addPath(pacman_dir);
        m_proxyInter->itemAdded(this, ARCH_KEY);
        emit checkUpdate();
    }
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
//            QMessageBox *about = new QMessageBox(QMessageBox::Information,
//                                             tr("About Arch Update"),
//                                             tr("Deepin Dock Plugin: Arch Update Indicator.\n"
//                                                "License: GPLv3.0\n"
//                                                "Author: CareF <me@mail.caref.xyz>\n"
//                                                "Source: https://github.com/CareF/deepin-dock-plugin-arch-update"),
//                                                 QMessageBox::Ok, m_items);
//            about->setIconPixmap(QIcon(":/lit").pixmap(64));

            DWIDGET_USE_NAMESPACE
            DAboutDialog *about = new DAboutDialog(m_items);
            about->setWindowTitle(tr("About Arch Update"));
            about->setProductName(tr("Deepin Dock Plugin: Arch Update"));
            about->setProductIcon(QIcon(":/lit"));
            about->setVersion(VERSION);
            about->setDescription(tr("Author: CareF <me@mail.caref.xyz>"));
            // void setCompanyLogo(const QPixmap &companyLogo);
            about->setWebsiteName(tr("Source Code (GitHub)"));
            about->setWebsiteLink("https://github.com/CareF/deepin-dock-plugin-arch-update");
            // void setAcknowledgementLink(const QString &acknowledgementLink);
            about->setLicense(tr("License: GPLv3.0"));

            about->setWindowModality(Qt::NonModal);
            about->show();
            // TODO: clickable url?
        }
    }
}

const QIntValidator ArchUpdatePlugin::TIMEINMIN(1, 99999);
void ArchUpdatePlugin::execSettingDiag() {
    // TODO: make it non-blocking!
    config[0].currentValue = m_data->check_cmd;
    config[1].currentValue = pacman_dir;
    config[2].currentValue = update_cmd;
    config[3].currentValue = QString::number(regularTimer.interval()/MINUTE);
    settingDialog->show();
}

void ArchUpdatePlugin::reloadSetting() {
    m_data->check_cmd = config[0].currentValue;
    m_proxyInter->saveValue(this, CHECK_CMD_KEY, config[0].currentValue);

    pacman_dir = config[1].currentValue;
    pacmanWatcher.removePaths(pacmanWatcher.files());
    pacmanWatcher.addPath(pacman_dir);
    m_proxyInter->saveValue(this, PACMAN_DIR_KEY, config[1].currentValue);

    update_cmd = config[2].currentValue;
    m_proxyInter->saveValue(this, UPDATE_CMD_KEY, config[2].currentValue);

    regularTimer.setInterval(MINUTE * config[3].currentValue.toInt());
    m_proxyInter->saveValue(this, CHK_INTERVAL_KEY, config[3].currentValue.toInt());
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

