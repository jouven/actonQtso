#ifndef ACTONQTSO_ACTIONFINISHED_HPP
#define ACTONQTSO_ACTIONFINISHED_HPP

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QJsonObject>
#include <unordered_map>

//this only "supports" one actionStringId because an action can have multiple
//checks (keep it simple)
class EXPIMP_ACTONQTSO actionFinishedCheck_c
{
public:
    //this applies to what's in the result fields immediately after the action has finished
    enum class actionExecutionResultFields_ec
    {
        outputStr = 0
        , errorStr = 1
        , externalOutputStr = 2
        , externalErrorStr = 3
        , returnCode = 5
        , endState = 6
        , startDateTime = 7
        , endDateTime = 8

    };
private:
    QString actionStringId_pri = 0;
    //FUTURE timeout setting?

    //if the check is successful on any kind of finish
    //option to make this check fail if the finished action is not successful
    //otherwise this check passes regardless how the action finished
    bool failCheckOnNotSuccessfulActionFinish_pri = true;

    std::unordered_map<actionExecutionResultFields_ec, QString> actionExecutionResultFieldToStringTrigger_pri;
public:
    actionFinishedCheck_c() = default;
    actionFinishedCheck_c(
            const QString& actionStringId_par_con
            , const bool failCheckOnNotSuccessfulActionFinish_par_con
            , const std::unordered_map<actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringTrigger_par_con
    );

    //keys are lower-case
    static EXPIMP_ACTONQTSO const QMap<QString, actionExecutionResultFields_ec> strToActionExecutionResultFieldsMap_sta_con;
    //values are camelcase
    static EXPIMP_ACTONQTSO const std::unordered_map<actionExecutionResultFields_ec, QString> actionExecutionResultFieldsToStrUMap_sta_con;

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString actionStringId_f() const;
    QString actionStringIdParsed_f() const;
    void setActionStringId_f(const QString& actionStringId_par_con);
    bool failCheckOnNotSuccessfulActionFinish_f() const;
    void setFailCheckOnNotSuccessfulActionFinish_f(const bool failCheckOnNotSuccessfulActionFinish_par_con);

    //on execution, will create a parser config using the string of the field of the action finished and a "key" string
    //can't be edited, so this works with only add and remove
    //won't replace existing keys in the parser configs
    //won't allow the use of keys already used in another checks because parser keys are unique
    //returns true if the mapping is added
    //use bool actonDataHub_c::hasStringKeyParserAnyDependency_f(const QString& stringKey_par_con) const; to check and warn
    //that a key is already used in another check
    bool mapActionResultToStringParserConfig_f(const actionExecutionResultFields_ec actionExecutionResultField_par_con, const QString& stringTrigger_par_con);
    void removeMapActionResultToStringParserConfig_f(const actionExecutionResultFields_ec actionExecutionResultField_par_con);

    std::unordered_map<actionExecutionResultFields_ec, QString> actionExecutionResultFieldToStringTrigger_f() const;
    void setActionExecutionResultFieldToStringTrigger_f(const std::unordered_map<actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringTrigger_par_con);
};

#endif // ACTONQTSO_ACTIONFINISHED_HPP
