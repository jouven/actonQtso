#ifndef ACTONQTSO_ACTIONFINISHED_HPP
#define ACTONQTSO_ACTIONFINISHED_HPP

#include "../checkData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <unordered_map>

class QJsonObject;
class textCompilation_c;
class actonDataHub_c;

class EXPIMP_ACTONQTSO actionFinishedData_c
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
protected:
    QString actionStringId_pro;
    //how many times the action has to finish successfully (which depends on successOnActionSuccess_pro)
    //to end this check (otherwise it will keep "executing")
    int_fast64_t finishedCount_pro = 1;
    //controls if this checks success depends on the action succcess or otherwise any action finished state
    bool successOnActionSuccess_pro = true;

    //TODO add, in the actionData_c, "checktimeout" field, if x time passes without the checks finishing finish the action with checks failed

    //type of result mapped to a string trigger
    std::unordered_map<actionExecutionResultFields_ec, QString> actionExecutionResultFieldToStringTrigger_pro;

    //prevent public assignments
    actionFinishedData_c& operator=(actionFinishedData_c const &) = default;
    actionFinishedData_c& operator=(actionFinishedData_c&) = default;
    actionFinishedData_c& operator=(actionFinishedData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    actionFinishedData_c(actionFinishedData_c const&) = default;
    actionFinishedData_c(actionFinishedData_c&) = default;

    actionFinishedData_c() = default;
    explicit actionFinishedData_c(
            const QString& actionStringId_par_con
            , const int_fast64_t finishedCount_par_con = 1
            , const bool successOnActionSuccess_par_con = true
            , const std::unordered_map<actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringTrigger_par_con = std::unordered_map<actionExecutionResultFields_ec, QString>()
    );

    QString actionStringId_f() const;
    QString actionStringIdParsed_f() const;
    void setActionStringId_f(const QString& actionStringId_par_con);

    //keys are lower-case
    static EXPIMP_ACTONQTSO const QMap<QString, actionExecutionResultFields_ec> strToActionExecutionResultFieldsMap_sta_con;
    //values are camelcase
    static EXPIMP_ACTONQTSO const std::unordered_map<actionExecutionResultFields_ec, QString> actionExecutionResultFieldsToStrUMap_sta_con;

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

    int_fast64_t finishedCount_f() const;
    void setFinishedCount_f(const int_fast64_t finishedCount_par_con);
    bool successOnActionSuccess_f() const;
    void setSuccessOnActionSuccess_f(const bool successOnActionSuccess_par_con);

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};

//this only "supports" one actionStringId because an action can have multiple
//checks (keep it simple)
class EXPIMP_ACTONQTSO actionFinishedCheck_c : public check_c, public actionFinishedData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidCheck_f(textCompilation_c* errors_par = nullptr) const override;

    //it will not clone actionExecutionResultFieldToStringTrigger_pro because it would create conflicts
    check_c* derivedClone_f() const override;

    baseCheckExecution_c* createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par) override;
    checkType_ec type_f() const override;
    QString typeStr_f() const override;

    bool derivedUpdateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con) override;
    bool derivedHasStringIdAnyDependency_f(const QString& stringId_par_con) const override;
    bool derivedUpdateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con) override;
    bool derivedHasStringTriggerAnyDependency_f(const QString& stringTrigger_par_con) const override;
    std::vector<QString> derivedStringTriggersInUse_f() const override;

public:
    actionFinishedCheck_c() = default;
    actionFinishedCheck_c(const checkData_c& checkData_par_con, const actionFinishedData_c& actionFinished_par_con);

    //updating certain action/checks objects might require to update other objects depending
    //if they have some kind of field dependency, i.e. stringId for actions, string trigger for actionFinished checks
    //this can be "solved" sending a signal if these fields change when saving the objects, but for triggers it would require to check all the parsed type fields
    //the actonDataHub ptr argument is there to connect stringTriggerChanged_signal to the function that updates stringTrigger dependencies (which is in actonDataHub_c)
    void updateActionFinishedData_f(const actionFinishedData_c& actionFinishedData_par_con, actonDataHub_c* actonDataHubPtr_par = nullptr);

Q_SIGNALS:
    void stringTriggerChanged_signal(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con);

};

#endif // ACTONQTSO_ACTIONFINISHED_HPP
