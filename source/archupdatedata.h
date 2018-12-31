#ifndef ARCHUPDATEDATA_H
#define ARCHUPDATEDATA_H 

#include <QDateTime>
#include <QObject>

class ArchUpdateData : public QObject
{
    Q_OBJECT

public:
    ArchUpdateData(const QString cmd="/usr/bin/checkupdates");
    bool is_checking() const {return ischecking;}
    int error_code() const {return error;}
    int newcount() const {return newpacks.count();}
    QString check_cmd;
    QDateTime lastcheck;

signals:
    void finished();

public slots:
    bool check(); // return true if success, otherwise false

private:
    bool ischecking;
    int error; // Return code for update command, 0 for no error
    // If the process cannot be started, error=-2. If the process crashes, error=-1.
    QList<QString> newpacks;
};

#endif // ARCHUPDATEDATA_H 
