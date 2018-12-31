#ifndef ARCHUPDATEDATA_H
#define ARCHUPDATEDATA_H 

#include <QDateTime>
#include <QObject>
#include <QStringList>

#define DEFAULT_CHK_UPDATE "/usr/bin/checkupdates"
#define DEFAULT_PACMAN_DIR "/var/lib/pacman/local"
#define DEFAULT_UPDATE "deepin-terminal -e sh -c \"sudo pacman -Syu ; echo Done - Press enter to exit;\""

class ArchUpdateData : public QObject
{
    Q_OBJECT

public:
    ArchUpdateData(const QString cmd=DEFAULT_CHK_UPDATE);
    bool is_checking() const {return ischecking;}
    int error_code() const {return error;}
    int newcount() const {return newpacks.count();}
    QString check_cmd;
    QDateTime lastcheck;
    const QStringList &newpackList() const {return newpacks;}

signals:
    void finished();

public slots:
    bool check(); // return true if success, otherwise false

private:
    bool ischecking;
    int error; // Return code for update command, 0 for no error
    // If the process cannot be started, error=-2. If the process crashes, error=-1.
    QStringList newpacks;
};

#endif // ARCHUPDATEDATA_H 
