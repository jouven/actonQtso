#ifndef ACTONQTSO_TIMER_HPP
#define ACTONQTSO_TIMER_HPP

#include "../checkData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QMap>

#include <unordered_map>

class QJsonObject;
class textCompilation_c;

class EXPIMP_ACTONQTSO timerData_c
{
public:
    enum class type_ec
    {
        empty = 0
        , timer = 1
        , dateTime = 2
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

    //keys are lower-case
    static EXPIMP_ACTONQTSO const QMap<QString, type_ec> strTotimerTypeMap_sta_con;
    //values are camelcase
    static EXPIMP_ACTONQTSO const std::unordered_map<type_ec, QString> timerTypeToStrUMap_sta_con;

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

    baseCheckExecution_c* createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par) override;
    checkType_ec type_f() const override;
    QString typeStr_f() const override;

public:
    timerCheck_c() = default;
    timerCheck_c(const checkData_c& checkData_par_con, const timerData_c& timer_par_con);

    void updateTimerData_f(const timerData_c& timerData_par_con);
};

#endif // ACTONQTSO_TIMER_HPP
