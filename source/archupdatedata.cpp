#include "archupdatedata.h"
#include <QProcess>
#include <QTextStream>
#include <QDebug>

ArchUpdateData::ArchUpdateData(const QString cmd):
    QObject(nullptr), check_cmd(cmd),
    lastcheck(QDateTime(QDate(1970,1,1))),
    ischecking(false), error(0) {

}

bool ArchUpdateData::check() {
    // This function can be time consumming.
    // so Make it an independent thread.
    if (ischecking)
        return false;
    ischecking = true;
#ifdef QT_DEBUG
    qDebug() << "Arch Update Plugin: Check Update Start!";
#endif
    QProcess chkprocess(this);
    chkprocess.start(check_cmd);
    if (!chkprocess.waitForStarted()) {
        error=-2;
        return false;
    }
    if (!chkprocess.waitForFinished()){
        error=-1;
        return false;
    }
    error = chkprocess.exitCode();
    if (error != 0) {
        qDebug() << "Check Update Error: " << chkprocess.readAllStandardOutput();
    }

    // Parse the result of checkupdates
    QTextStream outf(chkprocess.readAllStandardOutput());
    QString pkgname, v1, arrow, v2;
    newpacks.clear();
    while(!(outf>>pkgname>>v1>>arrow>>v2).atEnd()) {
        // format is "pkgname version1 -> version2" each line
        newpacks.push_back(pkgname);
    }

    lastcheck = QDateTime::currentDateTime();
    ischecking = false;
#ifdef QT_DEBUG
    qDebug() << "Arch Update Plugin: Check Update Finished!";
#endif
    emit finished();
    return true;
}
