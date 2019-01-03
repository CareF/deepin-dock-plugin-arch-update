#ifndef ARCHUPDATEPLUGIN_H
#define ARCHUPDATEPLUGIN_H

#include "dde-dock/pluginsiteminterface.h"
#include "archupdateitem.h"
#include "archupdateapplet.h"
#include "archupdatedata.h"
#include "settingdialog.h"

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QThread>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QThread>
#include <QIntValidator>

#define ARCH_KEY "arch-update-key"
#define STATE_KEY "enable"

// itemID or menuID for Context Menu.. Why can't I use enum and switch..
#define CHECKUPDATE "chk"
#define SETTINGS "set"
#define ABOUT "about"
// keys for settings from .config/deepin/dde-dock.conf, set and get through m_proxyInter
#define CHECK_CMD_KEY "check_command"
#define PACMAN_DIR_KEY "pacman_dir"
#define UPDATE_CMD_KEY "update_cmd"
#define CHK_INTERVAL_KEY "chk_intvl"

class ArchUpdatePlugin: public QObject, PluginsItemInterface {
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface"
            FILE "arch_update.json")

public:
    explicit ArchUpdatePlugin(QObject *parent = nullptr);
    virtual ~ArchUpdatePlugin() override;

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;
    void init(PluginProxyInterface *proxyInter) override;

    void pluginStateSwitched() override;
    bool pluginIsAllowDisable() override { return true; }
    bool pluginIsDisable() override;

    ///
    /// \brief itemWidget
    ///
    QWidget *itemWidget(const QString &itemKey) override;

    ///
    /// \brief itemTipWidget 
    /// Show number of updates and last check time when user hover the item
    ///
    QWidget *itemTipsWidget(const QString &itemKey) override;

    ///
    /// \brief show new packages when click the item
    ///
    virtual QWidget *itemPopupApplet(const QString &itemKey) override;

    ///
    /// \brief show setting menu when right click the item 
    ///
    const QString itemContextMenu(const QString &itemKey) override;

    ///
    /// \brief response to itemContextMenu
    ///
    void invokedMenuItem(const QString &itemKey, const QString &menuId, 
            const bool checked) override;

    int itemSortKey(const QString &itemKey) override;
    void setSortKey(const QString &itemKey, const int order) override;
    void refreshIcon(const QString &itemKey) override;

    void execSettingDiag();
    
signals:
    void checkUpdate();

private slots:
    void refreshTips();
    void fileChanged();
    void updatesystem();
    void reloadSetting();

private:
    QPointer<ArchUpdateItem> m_items;
    QPointer<ArchUpdateApplet> m_popups;
    QPointer<QLabel> m_tips;
    QPointer<ArchUpdateData> m_data;
    QThread m_updateThread;
    QString pacman_dir;
    QString update_cmd;
    QFileSystemWatcher pacmanWatcher;
    QTimer watcherTimer;
    QTimer regularTimer;
    static const QIntValidator TIMEINMIN;
    QList<settingItem> config;
    QPointer<SettingDialog> settingDialog;
};


#endif // ARCHUPDATEPLUGIN_H
