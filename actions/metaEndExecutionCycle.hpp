#ifndef ACTONQTSO_METAENDEXECUTIONCYCLE_HPP
#define ACTONQTSO_METAENDEXECUTIONCYCLE_HPP

#include "../actionData.hpp"

#include "../crossPlatformMacros.hpp"

class QJsonObject;

class EXPIMP_ACTONQTSO metaEndExecutionCycleData_c
{
public:
    enum class endType_ec
    {
        empty = 0
        //call stop for everything executing and prevent any non-executing, but waiting to execute, action to execute
        , stop = 1
        //waits for the current executing cycle to finish, looping actions (which ignore the executing cycle, will not loop again)
        , waitToFinish = 2
    };
protected:
    endType_ec endType_pro = endType_ec::stop;
    //when stoppping pass a timeout (in milliseconds) before killing
    //int_fast64_t killTimeout_pro = -1;

    //if stopping timeouts, kill after
    //the timeout value is set in execution options
    bool killAfterTimeout_pro = false;
    //prevent public assignments
    metaEndExecutionCycleData_c& operator=(metaEndExecutionCycleData_c const &) = default;
    metaEndExecutionCycleData_c& operator=(metaEndExecutionCycleData_c&) = default;
    metaEndExecutionCycleData_c& operator=(metaEndExecutionCycleData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    metaEndExecutionCycleData_c(metaEndExecutionCycleData_c const&) = default;
    metaEndExecutionCycleData_c(metaEndExecutionCycleData_c&) = default;

    metaEndExecutionCycleData_c() = default;
    explicit metaEndExecutionCycleData_c(
            const endType_ec type_par_con
            , const bool killAfterTimeout_par_con
    );

    //keys are lower-case
    static EXPIMP_ACTONQTSO const QMap<QString, endType_ec> strToEndExecutionTypeMap_sta_con;
    //values are camelcase
    static EXPIMP_ACTONQTSO const std::unordered_map<endType_ec, QString> endExecutionTypeToStrUMap_sta_con;


    endType_ec endType_f() const;
    void setEndType_f(const endType_ec& type_par_con);
//    int_fast64_t killTimeout_f() const;
//    void setKillTimeout_f(const int_fast64_t& killTimeout_par_con);
    bool killAfterTimeout_f() const;
    void setKillAfterTimeout_f(const bool killAfterTimeout_par_con);
//    bool isFieldsDataValid_f(textCompilation_c* = nullptr) const
//    {
//        return true;
//    }
};

class EXPIMP_ACTONQTSO metaEndExecutionCycleAction_c : public action_c, public metaEndExecutionCycleData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidAction_f(textCompilation_c* = nullptr) const override;

    //uint_fast64_t derivedUpdateActionStringIdDependencies_f(const QString& , const QString& ) override;
    //uint_fast64_t derivedActionStringIdDependencyCount_f(const QString& ) const override;
    //uint_fast64_t derivedStringTriggerCreationConflictCount_f(const QString& ) const override;
    //uint_fast64_t derivedUpdateStringTriggerDependecies_f(const QString& oldStringTrigger_par_con, const QString& newStringTrigger_par_con) override;
    //uint_fast64_t derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const override;
    //QSet<QString> derivedStringTriggerCreationCollection_f() const override;
    //QSet<QString> derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const override;

    action_c* derivedClone_f() const override;

    baseActionExecution_c* createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par) override;
    actionType_ec type_f() const override;
    //QString typeStr_f() const override;
public:
    metaEndExecutionCycleAction_c() = default;
    metaEndExecutionCycleAction_c(const actionData_c& actionData_par_con, const metaEndExecutionCycleData_c& metaEndExecutionData_par_con);

    void updateMetaEndExecutionCycleData_f(const metaEndExecutionCycleData_c& metaEndExecutionCycleData_par_con);
};

#endif // ACTONQTSO_METAENDEXECUTION_HPP
