#ifndef ACTONQTSO_CHECKDATA_HPP
#define ACTONQTSO_CHECKDATA_HPP

#include "checkMappings/checks.hpp"

#include "crossPlatformMacros.hpp"

#include <QObject>
#include <QString>
#include <QMap>

#include <vector>
#include <unordered_map>

class baseCheckExecution_c;
class checkDataExecutionResult_c;
class action_c;
class check_c;
class textCompilation_c;
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
public:
    enum class resultLogic_ec
    {
        empty = 0
        , trueOnSuccess = 1
        , trueOnFailure = 2
        //when it finishes in anyway without error/s
        , trueAlwaysExceptOnError = 3
        //only when it "finishes" with an error/s
        , trueOnError = 4
    };
protected:

    //NOT IN USE RIGHT NOW
    //QString uniqueIdString_pro;

    QString description_pro;

    //This option is not used by the check_c/checkData_c class but by the action checkDataHub_c
    //when multiple checks are executed:
    //true: executes the threaded in order but all will execute at the same time
    //false: execute the non-threaded checks one after the other (not at the same time)
    bool threaded_pro = false;
    //will ignore execution operations (execute, stop, execution objects from this obj will return nullptr)
    bool enabled_pro = true;
    //logic when evaluating the check result
    resultLogic_ec resultLogic_pro = resultLogic_ec::trueOnSuccess;

    //20190529 not used in validations anymore,
    //at some point running validations can grow
    //endlessly and it should be obvious that an object data shouldn't be modified
    //when it's executing operations reading its data,
    //i.e. sorting a vector and during the sort
    //changing elements, it will not end well
    //std library doesn't have this kind of validations
    //action_c* parentAction_pri = nullptr;

    //prevent public assignments
    checkData_c& operator=(checkData_c const &) = default;
    checkData_c& operator=(checkData_c&) = default;
    checkData_c& operator=(checkData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    checkData_c(checkData_c const&) = default;
    checkData_c(checkData_c&) = default;

    explicit checkData_c() = default;
    //see actionData_c explanation, also this is not a reference to not replace the default copy ctor
    //explicit checkData_c(const checkData_c* const source_par_con);
    explicit checkData_c(
            //const QString& uniqueIdString_par_con
            const QString& description_par_con
            , const bool threaded_par_con
            , const bool enabled_par_con
            , const resultLogic_ec resultLogic_par_con
    );

    static EXPIMP_ACTONQTSO const QMap<QString, resultLogic_ec> strToResultLogicMap_sta_con;
    static EXPIMP_ACTONQTSO const std::unordered_map<resultLogic_ec, QString> resultLogicToStrUMap_sta_con;

    bool isFieldsCheckDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};

class EXPIMP_ACTONQTSO check_c  : public QObject, public checkData_c
{
    Q_OBJECT
    //the id is a, fast, means to map the row position with the checkData
    int_fast64_t id_pri = 0;
    //this QObject can live on the main thread or on a different thread
    baseCheckExecution_c* checkDataExecution_ptr_pri = nullptr;
    //this QObject will live always on the main thread
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
    //to be able to call isFieldsDataValid_f (from each derived class base "data" class, i.e. actionFinishedData_c::isFieldsDataValid_f) from check_c
    virtual bool derivedIsValidCheck_f(textCompilation_c* errors_par = nullptr) const = 0;

    virtual check_c* derivedClone_f() const = 0;
    virtual bool derivedUpdateStringIdDependencies_f(const QString& , const QString& );
    virtual bool derivedHasStringIdAnyDependency_f(const QString& ) const;
    virtual bool derivedUpdateStringTriggerDependecies_f(const QString& , const QString& );
    virtual bool derivedHasStringTriggerAnyDependency_f(const QString& ) const;
    virtual std::vector<QString> derivedStringTriggersInUse_f() const;

    virtual baseCheckExecution_c* createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par) = 0;

protected:
    explicit check_c();

    explicit check_c(const checkData_c& checkData_par_con);

    //delete the pointer variables if they aren't nullptr
    ~check_c();
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

    //20191127 not in use because QObject has it's own setParent
    //when copying/asign/move an action, the "new" action checkDataHub calls this
    //void setParentAction_f(action_c* parentAction_par);


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
    checkData_c::resultLogic_ec resultLogic_f() const;


    void setDescription_f(const QString& description_par_con);
    void setUniqueIdString_f(const QString& uniqueIdString_par_con);
    void setThreaded_f(const bool threaded_par_con);
    void setEnabled_f(const bool enabled_par_con);
    void setResultLogic_f(const checkData_c::resultLogic_ec& resultLogic_par_con);

    //won't reexecute if already executing, check results object to know if execution has finished
    void tryExecute_f();
    //returns if true if it's stopping else false
    void stopExecution_f();
    //will initialize (new checkDataExecutionResult_pri) if it hasn't been initilized
    checkDataExecutionResult_c* createGetCheckDataExecutionResult_ptr_f();
    checkDataExecutionResult_c* checkDataExecutionResult_ptr_f() const;

    //action_c* parentAction_f() const;

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

    bool isFieldsCheckValid_f(textCompilation_c* errorsPtr_par = nullptr) const;

    void updateCheckData_f(const checkData_c& checkData_par_con);
private Q_SLOTS:
    void setCheckDataExecutionNull_f();
};

#endif // ACTONQTSO_CHECKDATA_HPP
