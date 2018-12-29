#ifndef ARCHUPDATEPLUGIN_H
#define ARCHUPDATEPLUGIN_H

#include "dde-dock/pluginsiteminterface.h"
#include "archupdatewidgets.h"
#include "archupdatedata.h"
#include <QWidget>
#include <QSettings>

class ArchUpdatePlugin: public QObject, PluginsItemInterface {
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface"
            FILE "arch_update.json")

public:
    explicit ArchUpdatePlugin(QObject *parent = nullptr);
    ~ArchUpdatePlugin();

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;
    void init(PluginProxyInterface *proxyInter) override;

    void pluginStateSwitched() override;
    bool pluginIsAllowDisable() override { return true; };
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
    const QWidget *itemPopupApplet(const QString &itemKey) override;

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
    
private slots:
    void checkUpdate();

private:
    QPointer<ArchUpdateItem> m_items;
    QPointer<ArchUpdateApplet> m_popups;
    QPointer<QLabel> m_tips;
    ArchUpdateData* m_data;
};


#endif // ARCHUPDATEPLUGIN_H
