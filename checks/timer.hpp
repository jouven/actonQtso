#ifndef ACTONQTSO_TIMER_HPP
#define ACTONQTSO_TIMER_HPP

#include "../checkData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QList>

class QJsonObject;
class textCompilation_c;

class EXPIMP_ACTONQTSO timerData_c
{
public:
    enum class type_ec
    {
        empty
        , timer
        , dateTime
    };
protected:
    type_ec type_pro = type_ec::timer;
    //timer or a datetime it can be saved as an int64
    //datetime values are saved as ms since epoch (UTC)
    int_fast64_t value_pro = 0;

    //this applies only to dateTime type, if the result of the timeout is negative because the datetime value is past (see timerExecution.cpp)
    //true = error-false the check
    //false = launch with 0 timeout (immediately)
    bool errorOnPastDateTimeValue_pro = true;
    //prevent public assignments
    timerData_c& operator=(timerData_c const &) = default;
    timerData_c& operator=(timerData_c&) = default;
    timerData_c& operator=(timerData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    timerData_c(timerData_c const&) = default;
    timerData_c(timerData_c&) = default;

    timerData_c() = default;
    explicit timerData_c(
            const type_ec type_par_con
            , const int_fast64_t value_par_con
            , const bool errorOnPastDatetimeValue_par_con
    );

    //strings not matching a type will return an empty type, case can be ignored because internally it's lowered before comparing
    static EXPIMP_ACTONQTSO type_ec stringTotimerType_f(const QString& str_par_con);
    static EXPIMP_ACTONQTSO QString timerTypeToString_f(const type_ec type_par_con);
    static EXPIMP_ACTONQTSO QList<QString> timerTypeStringValues_f();

    type_ec alarmType_f() const;
    void setAlarmType_f(const type_ec& alarmType_par_con);
    //ms since epoch (UTC)
    int_fast64_t value_f() const;
    void setValue_f(const int_fast64_t& value_par_con);
    bool errorOnPastDateTimeValue_f() const;
    void setErrorOnPastDateTimeValue_f(const bool errorOnPastDateTimeValue_par_con);

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};

class EXPIMP_ACTONQTSO timerCheck_c : public check_c, public timerData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidCheck_f(textCompilation_c* errors_par = nullptr) const override;

    check_c* derivedClone_f() const override;

    baseCheckExecution_c* createExecutionObj_f(checkExecutionResult_c* checkDataExecutionResult_ptr_par) override;
    checkType_ec type_f() const override;

    //uint_fast64_t derivedStringTriggerCreationConflictCount_f(const QString& stringTrigger_par_con) const override;
    //uint64_t derivedUpdateActionStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con) override;
    //uint64_t derivedActionStringIdDependencyCount_f(const QString& stringId_par_con) const override;
    //uint64_t derivedUpdateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con) override;
    //uint64_t derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const override;
    //QSet<QString> derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const override;
    //QSet<QString> derivedStringTriggerCreationCollection_f() const override;

    QString derivedReference_f() const override;
public:
    timerCheck_c() = default;
    timerCheck_c(const checkData_c& checkData_par_con, const timerData_c& timerData_par_con);

    void updateTimerData_f(const timerData_c& timerData_par_con);
};

#endif // ACTONQTSO_TIMER_HPP
