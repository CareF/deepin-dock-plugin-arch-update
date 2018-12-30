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
    // sMake it an independent thread.
    ischecking = true;
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
        qDebug() << chkprocess.readAllStandardOutput();
    }

    // Parse the result of checkupdates
    QTextStream outf(chkprocess.readAllStandardOutput());
    QString pkgname, v1, arrow, v2;
    newpacks.clear();
    while(!outf.atEnd()) {
        // format is "pkgname version1 -> version2" each line
        outf>>pkgname>>v1>>arrow>>v2;
        newpacks.push_back(pkgname);
    }

    lastcheck = QDateTime::currentDateTime();
    ischecking = false;
    return true;
}
