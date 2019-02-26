#ifndef ACTONQTSO_ACTIONFINISHED_HPP
#define ACTONQTSO_ACTIONFINISHED_HPP

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QJsonObject>

//this only "supports" one actionStringId because an action can have multiple
//checks (keep it simple)
class EXPIMP_ACTONQTSO actionFinishedCheck_c
{
    QString actionStringId_pri = 0;
    //FUTURE timeout setting?
    //if the check is successful on any kind of finish

    //option to make this check fail if the finished action is not successful
    //otherwise this check passes regardless how the action finished
    bool failCheckOnNotSuccessfulActionFinish_pri = true;

public:
    actionFinishedCheck_c() = default;
    actionFinishedCheck_c(
            const QString& actionStringId_par_con
            , const bool failCheckOnNotSuccessfulActionFinish_par_con
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);
    QString actionStringId_f() const;
    void setActionStringId_f(const QString& actionStringId_par_con);
    bool failCheckOnNotSuccessfulActionFinish_f() const;
    void setFailCheckOnNotSuccessfulActionFinish_f(const bool failCheckOnNotSuccessfulActionFinish_par_con);
};

#endif // ACTONQTSO_ACTIONFINISHED_HPP
