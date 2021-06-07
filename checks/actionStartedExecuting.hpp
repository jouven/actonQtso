#ifndef ACTONQTSO_ACTIONSTARTEDEXECUTING_HPP
#define ACTONQTSO_ACTIONSTARTEDEXECUTING_HPP

#include "../checkData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>

class QJsonObject;
class textCompilation_c;

//this check requires a parent action and the action is in a actonDataHub_c
//it won't work in the void and it requires the existence of other actions, being in actonDataHub_c, to work
//for the reasons above it "can't" be excuted "alone"

class EXPIMP_ACTONQTSO actionStartedExecutingData_c
{
protected:
    //the action stringId that need to start executing
    QString actionStringId_pro;
    //wait x milliseconds before finishing the check when the action started executing event is detected
    int_fast64_t waitFinishMs_pro = 0;

    //prevent public assignments
    actionStartedExecutingData_c& operator=(actionStartedExecutingData_c const &) = default;
    actionStartedExecutingData_c& operator=(actionStartedExecutingData_c&) = default;
    actionStartedExecutingData_c& operator=(actionStartedExecutingData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    actionStartedExecutingData_c(actionStartedExecutingData_c const&) = default;
    actionStartedExecutingData_c(actionStartedExecutingData_c&) = default;

    actionStartedExecutingData_c() = default;
    explicit actionStartedExecutingData_c(
            const QString& actionStringId_par_con
            , const uint_fast64_t delayMs_par_con = 0
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString actionStringId_f() const;
    //QString actionStringIdParsed_f() const;
    void setActionStringId_f(const QString& actionStringId_par_con);

    int_fast64_t waitFinishMs_f() const;
    void setWaitFinishMs_f(const int_fast64_t& waitFinishMs_par_con);

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};


class EXPIMP_ACTONQTSO actionStartedExecutingCheck_c : public check_c, public actionStartedExecutingData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidCheck_f(textCompilation_c* errors_par = nullptr) const override;

    check_c* derivedClone_f() const override;

    baseCheckExecution_c* createExecutionObj_f(checkExecutionResult_c* checkDataExecutionResult_ptr_par) override;
    checkType_ec type_f() const override;

    //uint_fast64_t derivedStringTriggerCreationConflictCount_f(const QString& stringTrigger_par_con) const override;
    uint_fast64_t derivedUpdateActionStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con) override;
    uint_fast64_t derivedActionStringIdDependencyCount_f(const QString& stringId_par_con) const override;
    //uint64_t derivedUpdateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con) override;
    //uint64_t derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const override;
    //QSet<QString> derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const override;
    //QSet<QString> derivedStringTriggerCreationCollection_f() const override;

    QString derivedReference_f() const override;
public:
    actionStartedExecutingCheck_c() = default;
    actionStartedExecutingCheck_c(const checkData_c& checkData_par_con, const actionStartedExecutingData_c& actionStartedExecuting_par_con);

    void updateAcionStartedExecutingData_f(const actionStartedExecutingData_c& actionStartedExecutingData_par_con);
};

#endif // ACTONQTSO_ACTIONSTARTEDEXECUTING_HPP
