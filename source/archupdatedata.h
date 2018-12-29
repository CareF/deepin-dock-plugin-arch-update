#ifndef ARCHUPDATEDATA_H
#define ARCHUPDATEDATA_H 

#include <QDateTime>

#define ARCH_KEY "arch-update-key"
#define STATE_KEY "enable"

struct ArchUpdateData {
    int newpacks;
    QDateTime lastcheck;
};

#endif // ARCHUPDATEDATA_H 
