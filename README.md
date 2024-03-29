Arch Linux Updates Indicator DDE Dock Plugin
===============


**This repo is no longer actively maintaining, due to breaking changes from deepin and lack of API docs**

Update indicator for Arch Linux and Deepin Dock, 

<img src="/updateApplet.png" width=350>

The plugin uses dtkwidget to get full deepin design experience. 

<img src="/settings.png" width=450>

Depend on `pacman-contrib`

Install
--------

### AUR ###
[deepin-plugin-arch-update](https://aur.archlinux.org/packages/deepin-dock-plugin-arch-update/)

### Compile it locally ###
Require Qt and dtkwidget

```
mkdir build
cd build
qmake ../source
make
sudo make install
pkill dde-dock
```

### Note on i18n ###
Some of Qt's widgets depends on qtbase_*.qm to fully translated. Unfortunately qtbase_zh_CN is missing. There's a third-party translation [here](https://github.com/wisaly/qtbase_zh). copy qtbase_zh_CN.qm to `/usr/share/qt/translations` to have full Chinese experience. 

Qt 自带的一些组件需要 `qtbase_*.qm` 文件来实现翻译. 然而 `qtbase_zh_CN.qm` 缺失.. 不过可以从[这里](https://github.com/wisaly/qtbase_zh) 下载并复制到 `/usr/share/qt/translations` 实现完整的翻译. 

Changelog
-----------
- v2.3: Add auto hide and bug fix
- v2.2: hover tip for version change
- v2.1: bug fix
- v2.0: update to API version 1.1.1, icon becomes white in Efficient mode and replace Qt's QDialog with dtkwidget's DDialog
- v1.1: add i18n support

TODO
-------
- [ ] Watched package list
- [ ] System notice bubble
- [X] Hide on no package 
- [X] Auto Hide notice (how to revert setting)
- [X] Publish on AUR with Github Hook (?)
- [X] non-blocking dialog
- [X] translation (waiting for qtbase_zh_CN)
- [X] show update version infos
- [ ] add travis.ci support (can I?)

Other notes
-------
Not sure if it's a bug of Qt or not, but if I name qrc file `icons.qrc`, it will not be able to find the resource files. 
It will be appreciated if anyone can help me with why. 

Credits
----------
All icons are based on Thayer Williams' Archer logo, winner of Arch Linux logo contest.

Some portions are inspired by 
[arch-update](https://github.com/RaphaelRochet/arch-update), 
[Deepin official plugins](https://github.com/linuxdeepin/dde-dock/tree/master/plugins) 
and [CMDU](https://github.com/sonichy/CMDU_DDE_DOCK)
