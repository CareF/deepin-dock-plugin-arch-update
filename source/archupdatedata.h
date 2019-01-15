#ifndef ARCHUPDATEDATA_H
#define ARCHUPDATEDATA_H 

#include <QDateTime>
#include <QObject>
#include <QStringList>

#define DEFAULT_CHK_UPDATE "/usr/bin/checkupdates"
#define DEFAULT_PACMAN_DIR "/var/lib/pacman/local"
#define DEFAULT_UPDATE "deepin-terminal -e sh -c \"sudo pacman -Syu\""
#define DEFAULT_INTERVAL 30 //min
class ArchUpdatePlugin;

class ArchUpdateData : public QObject
{
    Q_OBJECT

public:
    ArchUpdateData(const QString cmd=DEFAULT_CHK_UPDATE);
    bool is_checking() const {return ischecking;}
    /** \brief Return code for update command, 0 for no error
     * \details
     * If the process cannot be started, error=-2.
     * If the process crashes, error=-1. */
    int error_code() const {return error;}
    int newcount() const {return newpacks.count();}
    QString check_cmd;
    const QStringList &newpackList() const {return newpacks;}
    const QString &currentV(int n) const {return versionOld[n];}
    const QString &newV(int n) const {return versionNew[n];}
    const QDateTime &getLastcheck() const {return lastcheck;}

friend class ArchUpdatePlugin;

signals:
    void finished();

public slots:
    bool check(); // return true if success, otherwise false

private:
    bool ischecking;
    int error;
    QDateTime lastcheck;
    QStringList newpacks;
    QStringList versionOld;
    QStringList versionNew;
};

#endif // ARCHUPDATEDATA_H 
