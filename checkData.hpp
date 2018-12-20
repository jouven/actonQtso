#ifndef ACTONQTSO_CHECKDATA_HPP
#define ACTONQTSO_CHECKDATA_HPP

#include "checkMappings/checks.hpp"

#include "crossPlatformMacros.hpp"

//#include <QObject>
#include <QString>
#include <QJsonObject>

class baseCheckExecution_c;
class checkDataExecutionResult_c;
class actionData_c;

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

//FUTURE? detect is checks are on the same storage and do them sequentially, else all at the same time

class EXPIMP_ACTONQTSO checkData_c
{
    //the id is a, fast, means to map the row position with the checkData
    int_fast64_t id_pri = 0;

    checkType_ec type_pri = checkType_ec::empty;
    //NOT IN USE RIGHT NOW
    QString uniqueIdString_pri;
    QString description_pri;

    //true: will run in order but at the same time as other checks
    //false: along the other non-threaded checks will run one after the other.
    //This option is not used by the checkData_c class but by checkDataHub_c
    //when multiple checks are executed, false makes checkDataHub_c execute
    //the non-threaded checks one after the other
    bool threaded_pri = false;
    //will ignore execution operations (execute, stop, execution objects, from this obj, will return Q_NULLPTR)
    bool enabled_pri = true;

    //bellow is not "jsoned"
    QJsonObject checkDataJSON_pri;

    baseCheckExecution_c* checkDataExecution_ptr_pri = nullptr;
    checkDataExecutionResult_c* checkDataExecutionResult_ptr_pri = nullptr;

    actionData_c* parentAction_pri = nullptr;

    void deleteCheckDataExecutionObject_f();
    void deleteCheckDataExecutionResultObject_f();

    void deleteExecutionObjects_f();
    void deleteUsedPtrs_f();

    void prepareToRun_f();
    void execute_f();

    bool isEditable_f() const;
    //use only on sets
    bool tryClearResultsOnEdit_f();
public:
    explicit checkData_c();
    explicit checkData_c(
            const checkType_ec type_par_con
            , const QString& uniqueIdString_par_con
            , const QString& description_par_con
            , const bool threaded_par_con = false
            , const QJsonObject& checkDataJSON_par_con = QJsonObject()
    );

    //copies everything except pointers
    checkData_c(const checkData_c& from_par_con);
    //required to copy from a pointer dereference, does the same the above
    checkData_c(checkData_c& from_par);
    //std::moves everything and nulls from_par pointers
    checkData_c(checkData_c&& from_par) noexcept;

    //copies everything except pointers and id_pri, deletes pointer objs
    checkData_c& operator=(const checkData_c& from_par_con);
    checkData_c& operator=(checkData_c& from_par);
    //same as move ctor
    checkData_c& operator=(checkData_c&& from_par) noexcept;

    //when copying/asign/move an action, the "new" action checkDataHub calls this
    void setParentAction_f(actionData_c* parentAction_par);
    //delete the pointer variables if they aren't Q_NULLPTR
    ~checkData_c();

    void write_f(QJsonObject &json) const;
    void read_f(const QJsonObject &json);

    //FUTURE dates (creation, modification with the option of hide/show in the grid)
    int_fast64_t id_f() const;
    checkType_ec type_f() const;
    QString uniqueIdString_f() const;
    QString description_f() const;
    bool threaded_f() const;
    bool enabled_f() const;
    QJsonObject checkDataJSON_f() const;
    //QJsonObject& checkDataJSON_ref_f();
    bool isExecuting_f() const;
    bool isStoppingExecution_f() const;

    void setType_f(const checkType_ec type_par_con);
    void setDescription_f(const QString& description_par_con);
    void setCheckDataJSON_f(const QJsonObject& checkDataJSON_par_con);
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

    actionData_c* parentAction_f() const;

    //update all the actions/checks setting that depend on an actionStringId
    //returns true if a field was updated (did match with the oldStringId)
    bool updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con);
    bool hasStringIdAnyDependency_f(const QString& stringId_par_con) const;
};

#endif // ACTONQTSO_CHECKDATA_HPP
