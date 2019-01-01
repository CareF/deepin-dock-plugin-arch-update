Arch Linux Updates Indicator DDE Dock Plugin
===============
Update indicator for Arch Linux and Deepin Dock, 

Depend on `pacman-contrib`

Install
--------

```
mkdir build
cd build
qmake ../source
make
sudo make install
pkill dde-dock
```

TODO
-------
- [ ] non-blocking dialog
- [ ] translation 
- [ ] show update version infos
- [ ] add travis.ci support (can I)


Known bugs
-----
- [ ] HiDi Scaling

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
