#include "actionFinished.hpp"

QString actionFinishedCheck_c::actionStringId_f() const
{
    return actionStringId_pri;
}

void actionFinishedCheck_c::setActionStringId_f(const QString& actionStringId_par_con)
{
    actionStringId_pri = actionStringId_par_con;
}

bool actionFinishedCheck_c::failCheckOnNotSuccessfulActionFinish_f() const
{
    return failCheckOnNotSuccessfulActionFinish_pri;
}

void actionFinishedCheck_c::setFailCheckOnNotSuccessfulActionFinish_f(const bool failCheckOnNotSuccessfulActionFinish_par_con)
{
    failCheckOnNotSuccessfulActionFinish_pri = failCheckOnNotSuccessfulActionFinish_par_con;
}

actionFinishedCheck_c::actionFinishedCheck_c(
        const QString& actionStringId_par_con
        , const bool failCheckOnNotSuccessfulActionFinish_par_con
)
    : actionStringId_pri(actionStringId_par_con)
    , failCheckOnNotSuccessfulActionFinish_pri(failCheckOnNotSuccessfulActionFinish_par_con)
{}

void actionFinishedCheck_c::write_f(QJsonObject& json_par) const
{
    json_par["actionStringId"] = actionStringId_pri;
    json_par["failCheckOnNotSuccessfulActionFinish"] = failCheckOnNotSuccessfulActionFinish_pri;
}

void actionFinishedCheck_c::read_f(const QJsonObject& json_par_con)
{
    actionStringId_pri = json_par_con["actionStringId"].toString();
    failCheckOnNotSuccessfulActionFinish_pri = json_par_con["failCheckOnNotSuccessfulActionFinish"].toBool();
}
