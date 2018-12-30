#ifndef ARCHUPDATEPLUGIN_H
#define ARCHUPDATEPLUGIN_H

#include "dde-dock/pluginsiteminterface.h"
#include "archupdateitem.h"
#include "archupdateapplet.h"
#include "archupdatedata.h"

#include <QThread>
#include <QObject>
#include <QWidget>
#include <QLabel>

#define ARCH_KEY "arch-update-key"
#define STATE_KEY "enable"

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
    
signals:
    void checkUpdate();

private slots:
    void updateTips();

private:
    QPointer<ArchUpdateItem> m_items;
    QPointer<ArchUpdateApplet> m_popups;
    QPointer<QLabel> m_tips;
    QPointer<ArchUpdateData> m_data;
    QThread m_updateThread;
    QString pacman_dir;
};


#endif // ARCHUPDATEPLUGIN_H
