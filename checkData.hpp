#ifndef ACTONQTSO_CHECKDATA_HPP
#define ACTONQTSO_CHECKDATA_HPP

#include "checkMappings/checks.hpp"

#include "crossPlatformMacros.hpp"

#include <QObject>
#include <QString>

#include <vector>

class baseCheckExecution_c;
class checkDataExecutionResult_c;
class action_c;
class check_c;

class QJsonObject;

//a check is "like an action" that leaves things unchanged but gets a binary result checking something: like if a file exist, hashes matching, something has finished,
//return code equal to a value... etc
//a check can retry (option), it can wait for an action to finish (dependency)
//this way dependencies can be made, using a check to wait for a certain action to finish, even to check if for a result and then run the action where that check is

//checks can be copied from another action (for now all of them, in the FUTURE allow to select which ones)

//checks aren't unique

//there can more than one check/s per action
//there must the ability to change the check order
//to decide what "is" a fail, allow initially: 1 if one fails "false result" (at least one must fail) and  2 if all fail "false result" (all must fail)
//thre above line can be read: 1 if all the checks return true the action will run 2 if all the checks return false the action won't be run

//FUTURE? detect is checks are on the same storage/disk and do them sequentially, else all at the same time

class EXPIMP_ACTONQTSO checkData_c
{
protected:
    //the id is a, fast, means to map the row position with the checkData
    int_fast64_t id_pro = 0;

    //NOT IN USE RIGHT NOW
    QString uniqueIdString_pro;
    QString description_pro;

    //true: will run in order but at the same time as other checks
    //false: along the other non-threaded checks will run one after the other.
    //This option is not used by the checkData_c class but by checkDataHub_c
    //when multiple checks are executed, false makes checkDataHub_c execute
    //the non-threaded checks one after the other
    bool threaded_pro = false;
    //will ignore execution operations (execute, stop, execution objects, from this obj, will return nullptr)
    bool enabled_pro = true;

    //20190529 not used in validations anymore,
    //at some point running validations can grow
    //endlessly and it should be obvious that an object data shouldn't be modified
    //when it's executing operations reading its data,
    //i.e. sorting a vector and during the sort
    //changing elements, it will not end well
    //std library doesn't have this kind of validations
    action_c* parentAction_pri = nullptr;
public:
    explicit checkData_c() = default;
    //see actionData_c explanation
    explicit checkData_c(const checkData_c& source_par_con);
    explicit checkData_c(
            const QString& uniqueIdString_par_con
            , const QString& description_par_con
            , const bool threaded_par_con
            , const bool enabled_par_con
    );
};

class EXPIMP_ACTONQTSO check_c  : public QObject, public checkData_c
{
    baseCheckExecution_c* checkDataExecution_ptr_pri = nullptr;
    checkDataExecutionResult_c* checkDataExecutionResult_ptr_pri = nullptr;

    void deleteCheckDataExecutionObject_f();
    void deleteCheckDataExecutionResultObject_f();

    void deleteExecutionObjects_f();
    void deleteUsedPtrs_f();

    void prepareToRun_f();
    void execute_f();

    bool isEditable_f() const;
    //use only on sets
    bool tryClearResultsOnEdit_f();

    virtual void derivedWrite_f(QJsonObject &json_ref_par) const = 0;
    virtual void derivedRead_f(const QJsonObject &json_par_con) = 0;

    virtual check_c* derivedClone_f() const = 0;
    virtual bool derivedUpdateStringIdDependencies_f(const QString& , const QString& );
    virtual bool derivedHasStringIdAnyDependency_f(const QString& ) const;
    virtual bool derivedUpdateStringTriggerDependecies_f(const QString& , const QString& );
    virtual bool derivedHasStringTriggerAnyDependency_f(const QString& ) const;
    virtual std::vector<QString> derivedStringTriggersInUse_f() const;

    virtual baseCheckExecution_c* createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par) = 0;
protected:
    explicit check_c() = default;
    explicit check_c(
            const checkData_c& checkData_par_con
    );
public:
//    //copies everything except pointers
//    check_c(const check_c& from_par_con);
//    //required to copy from a pointer dereference, does the same the above
//    check_c(check_c& from_par);
//    //std::moves everything and nulls from_par pointers
//    check_c(check_c&& from_par) noexcept;

//    //copies everything except pointers and id_pri, deletes pointer objs
//    check_c& operator=(const check_c& from_par_con);
//    check_c& operator=(check_c& from_par);
//    //same as move ctor
//    check_c& operator=(check_c&& from_par) noexcept;

    //when copying/asign/move an action, the "new" action checkDataHub calls this
    void setParentAction_f(action_c* parentAction_par);
//    //delete the pointer variables if they aren't Q_NULLPTR
//    ~check_c();

    void write_f(QJsonObject &json_ref_par) const;
    void read_f(const QJsonObject &json_par_con);

    virtual checkType_ec type_f() const = 0;
    virtual QString typeStr_f() const = 0;

    //FUTURE dates (creation, modification with the option of hide/show in the grid)
    int_fast64_t id_f() const;
    QString uniqueIdString_f() const;
    QString description_f() const;
    bool threaded_f() const;
    bool enabled_f() const;
    bool isExecuting_f() const;
    bool isStoppingExecution_f() const;

    void setDescription_f(const QString& description_par_con);
    void setUniqueIdString_f(const QString& uniqueIdString_par_con);
    void setThreaded_f(const bool threaded_par_con);
    void setEnabled_f(const bool enabled_par_con);

    //won't reexecute if already executing, check results object to know if execution has finished
    void tryExecute_f();
    //returns if true if it's stopping else false
    void stopExecution_f();
    //will initialize (new checkDataExecutionResult_pri) if it hasn't been initilized
    checkDataExecutionResult_c* createGetCheckDataExecutionResult_ptr_f();
    checkDataExecutionResult_c* checkDataExecutionResult_ptr_f() const;

    action_c* parentAction_f() const;

    //update all the actions/checks setting that depend on an actionStringId
    //returns true if a field was updated (did match with the oldStringId)
    bool updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con);
    bool hasStringIdAnyDependency_f(const QString& stringId_par_con) const;

    //in the checkdatahub it's an int but here it's a bool because it can only be one dependency
    bool updateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con);
    bool hasStringTriggerAnyDependency_f(const QString& stringTrigger_par_con) const;
    //although the return value is a vector, it will only contain unique strings
    std::vector<QString> stringTriggersInUse_f() const;

    //can return nullptr, if the json type doesn't match anything expected
    static check_c* readCreateDerived_f(const checkType_ec checkType_par_con);
    //aka the copy ctor alternative for polymorphic classes
    check_c* clone_f() const;
};

#endif // ACTONQTSO_CHECKDATA_HPP
