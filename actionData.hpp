//helper class/s to deal with the shortcomings of QTableWidget,
//because it only deals with rows and cells, it doesn't detect duplicates,
//it doesn't "map" where is anything, uses QVariant as underlying value,
//so... lets skip that, the Data and setData, and just use some extra mappings and have more functionality

#ifndef ACTONQTSO_ACTIONDATA_HPP
#define ACTONQTSO_ACTIONDATA_HPP

#include "actionMappings/actions.hpp"
#include "actionMappings/actionExecutionStates.hpp"
#include "checksDataHub.hpp"

//#include "baseClassQtso/baseClassQt.hpp"
#include "crossPlatformMacros.hpp"

#include <QObject>
#include <QString>

#include <vector>

//TODO implement a field widget that has field name, source (combo), and field value
//i.e: directory to create: (field name), from action result/manual input (source), action unique id/"/home/jouven/test" (value)

//FUTURE make description optional, for each action create a description function that can be used some generic description
//that use values of that specific action as a "description"

class actionDataExecutionResult_c;
class baseActionExecution_c;
class QJsonObject;

//this class is a split from actionData_c to make it easier for the
//classes inheriting from actionData_c to ctor the base class
//it is the data part (most of) that will get jsoned on file save
class EXPIMP_ACTONQTSO actionData_c
{
protected:
    //the id is a, fast, means to map the row position with the actionData
    int_fast64_t id_pro = 0;
    //can't be empty and should/must be unique (actionDataHub_c checks this)
    QString stringId_pro;

    QString description_pro;

    //the following 2 bools, only apply if there are >0 checks

    //all the check results should be *logic and* as a final result
    //otherwise it's *or logic* (one successful check is enough to run the action)
    bool checkResultLogicAnd_pro = true;
    //when the above is false and one result is successful but not all the checks
    //have run finish already executing but don't run any check left (it's not required anyway)
    bool runAllChecksAnyway_pro = false;

    //will ignore execution operations (execute, stop, execution objects, from this obj, will return Q_NULLPTR)
    bool enabled_pro = true;

    bool checksEnabled_pro = true;

    //20190121 removed the global option (in the executionOptions_c class) and added here
    //to allow more granular control.
    //Stops all the execution when any actions with this option "true" has an error,
    //otherwise actions with this option "false" won't stop the global execution.
    //Actions with check type "finishedExecution" might need extra care
    //because of the option "failCheckOnNotSuccessfulActionFinish"
    bool stopAllExecutionOnError_pro = true;

    checksDataHub_c checkDataHub_pro;

public:

    //checkDataHub_pri will have no parent (=nullptr)
    //just to make actionData_c& source_par_con = actionData_c() work
    explicit actionData_c();
    //explanation time
    //why is this here? TLDR for derived classes
    //this class is inherited from an abstract class, the abstract class can't be ctored
    //what is ctored are the the derived classes,
    //so to not increase the derived classes ctor this is done
    //the variables here are the DATA part of what would/should? be in the abstract class
    explicit actionData_c(action_c* actionPtr_par, const actionData_c& source_par_con = actionData_c());
    //this is to ctor this object alone, checkDataHub_pri will have no parent (=nullptr)
    //checkDataHub_pri parent type is action_c (not actionData_c)
    explicit actionData_c(
            const QString& stringId_par_con
            , const QString& description_par_con
            , const bool checkResultLogicAnd_par_con
            , const bool runAllChecksAnyway_par_con
            , const bool enabled_par_con
            , const bool checksEnabled_par_con
            , const bool stopAllExecutionOnError_par_con
    );
};

//an action or actionData is:
//what goes in a row in the mainWindow in actonQtg
//But more seriously it's an operation, creating a folder, deleting a file... that has a result/s
//the result of an action doesn't matter to the action or other actions "directly"
//(an action can use other action results but without knowing if there is any, it can't know),
//a "check" can query if an action, has ended (or not), if it's successful (or not), if there are results (or not)
//an action can have multiple checks
class EXPIMP_ACTONQTSO action_c : public QObject, public actionData_c //: public eines::baseClassQt_c
{
    //it's a baseActionExecution_c (base class) because every execution object
    //has a different class but all have baseActionExecution_c as base and using the action type
    //it's possible to cast to the different execution classes or use virtual functions
    baseActionExecution_c* actionDataExecution_ptr_pri = nullptr;
    actionDataExecutionResult_c* actionDataExecutionResult_ptr_pri = nullptr;

    void clearExecutionResults_f() const;

    void deleteActionDataExecutionObject_f();
    void deleteActionDataExecutionResultObject_f();

    void execute_f();
    void examineCheckResults_f(std::vector<check_c*> checksRun_par);
    void prepareToRunAction_f();

    bool isKillingExecutionAfterTimeout_pri = false;

    void stopExecutingChecks_f();
    //must be called after tryStopExecution_f
    void kill_f();

    bool isEditable_f() const;
    //use only on sets
    bool tryClearResultsOnEdit_f();

    virtual void derivedWrite_f(QJsonObject &json_ref_par) const = 0;
    virtual void derivedRead_f(const QJsonObject &json_par_con) = 0;

    virtual action_c* derivedClone_f() const = 0;

    virtual baseActionExecution_c* createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par) = 0;

protected:
    explicit action_c();
    explicit action_c(
            const actionData_c& actionData_par_con
    );
public:
    void read_f(const QJsonObject &json_par_con);
    void write_f(QJsonObject &json_ref_par) const;

    virtual actionType_ec type_f() const = 0;
    virtual QString typeStr_f() const = 0;
    //FUTURE dates (creation, modification with the option of hide/show in the grid)
    int_fast64_t id_f() const;
    QString stringId_f() const;
    QString description_f() const;
    bool checksEnabled_f() const;
    bool enabled_f() const;
    //stops all execution if this action has an error
    bool stopExecutionOnError_f() const;

    checksDataHub_c* checkDataHub_ptr_f();
    const checksDataHub_c& checkDataHub_f() const;

    bool isStoppingExecution_f() const;
    bool isExecuting_f() const;
    bool killingExecutionAfterTimeout_f() const;

    int_fast32_t setStringId_f(const QString& stringId_par_con, const bool updateFieldsThatUsedOldValue_par_con = false);
    void setDescription_f(const QString& description_par_con);
    void setChecksEnabled_f(const bool checksEnabled_par_con);
    void setEnabled_f(const bool enabled_par_con);
    void setStopExecutionOnError_f(const bool stopExecutionOnError_par_con);

    //WARNING actionDataHub_c (datahub_f) has an execute function too, executeActionDataRows_f
    //Use that call if the Action is in the datahub, only call this if the action is in the "void"
    //won't rerun if already executing, check or connect the results object
    //to know if/when execution has finished
    void tryExecute_f();
    //stops initial or executing checks action, for more expeditive options try kill
    //returns if true if it's stopping else false
    //FUTURE revamp, if an action is told to stop all the the actions that depend on this one should stop too
    //right now this happens because everything stops too (if used from the actonDataHub obj)
    //kill timeout can be set in actonDataHub (actonExecutionOptions)
    void tryStopExecution_f(const bool killAfterTimeout_par_con = false);
    //will initialize (new actionDataExecutionResult_pri) if it hasn't been initilized
    actionDataExecutionResult_c* createGetActionDataExecutionResult_ptr_f();
    //check against Q_NULLPTR to know if exists
    actionDataExecutionResult_c* actionDataExecutionResult_ptr_f() const;
    //update all the actions/checks setting that depend on an actionStringId
    //returns the number of updated checks/action properties which did match with the oldStringId
    int_fast32_t updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con);
    bool hasStringIdAnyDependency_f(const QString& stringId_par_con) const;

    //below 3 functions, only used in actionFinished checks for now
    int_fast32_t updateStringTriggerParserDependencies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con);
    bool hasStringTriggerParserAnyDependency_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par) const;
    //although the return value is a vector, it will only contain unique strings
    std::vector<QString> stringTriggersInUse_f() const;

    //can return nullptr, if the json type doesn't match anything expected
    static action_c* readCreateDerived_f(const actionType_ec actionType_par_con);
    //aka the copy ctor alternative for polymorphic classes
    action_c* clone_f() const;
};

#endif // ACTONQTSO_ACTIONDATA_HPP
