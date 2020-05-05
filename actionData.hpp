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
#include <QSet>

#include <vector>

//FUTURE make description optional, for each action create a description function that can be used some generic description
//that use values of that specific action as a "description"

class actionDataExecutionResult_c;
class baseActionExecution_c;
class QJsonObject;
class textCompilation_c;

//this class is a split from actionData_c to make it easier for the
//classes inheriting from actionData_c to ctor the base class
//it is the data part (most of) that will get jsoned on file save
class EXPIMP_ACTONQTSO actionData_c
{
protected:

    //FUTURE? make stringIdParsed_f? (when I make more meta actions)
    //can't be empty and should/must be unique (actionDataHub_c checks this)
    QString stringId_pro;

    QString description_pro;

    //TODO? if I add options to the checks in general (in checkDataHub_c?) move this there
    //all the check results should be *logic and* as a final result
    //otherwise it's *or logic*: one successful check is enough to run the action
    //this also means that when the first successful check happens the rest are "stopped"
    bool checkResultLogicAnd_pro = true;

    //will ignore execution operations (execute, stop, execution objects, from this obj, will return Q_NULLPTR)
    bool enabled_pro = true;

    bool checksEnabled_pro = true;

    //20190121 removed the global option (in the executionOptions_c class) and added here
    //to allow more granular control.
    //Stops all the execution when any actions with this option "true" has an error,
    //actions with this option "false" won't stop the global execution.
    bool stopAllExecutionOnError_pro = true;

    //this implies:
    //1 this action will keep being launched again after it finishes
    //2 this action ignores "execution options" loop setting
    //3 this action checks can be tricky, like actionFinished,
    //  this check can get multiple success/s from an action which finished just once (which migth execute again but other stuff has to happen first)
    //4 this action can only be stopped
    //5 a "normal" the execution cycle will never end (because this action will keep it going)
    bool repeatExecution_pro = false;

    //actionData_c(actionData_c const&) = default;
    //prevent public assignments
    actionData_c& operator=(actionData_c const &) = default;
    actionData_c& operator=(actionData_c&) = default;
    actionData_c& operator=(actionData_c&&) = default;

public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    actionData_c(actionData_c const&) = default;
    actionData_c(actionData_c&) = default;

    explicit actionData_c() = default;

    explicit actionData_c(
            const QString& stringId_par_con
            , const QString& description_par_con
            , const bool checkResultLogicAnd_par_con
            , const bool enabled_par_con
            , const bool checksEnabled_par_con
            , const bool stopAllExecutionOnError_par_con
            , const bool repeatExecution_par_con
    );

    bool isFieldsActionDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;

    QString stringId_f() const;
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
    Q_OBJECT

    //the id is a, fast, means to map the row position with the action
    int_fast64_t id_pri;

    //DATA
    checksDataHub_c checkDataHub_pri;
    //this object can live on the main thread or on a different thread

    //CONTROL
    //this QObject can live on the main thread or on a different thread
    baseActionExecution_c* actionDataExecution_ptr_pri = nullptr;
    //this QObject will always live on the main thread
    actionDataExecutionResult_c* actionDataExecutionResult_ptr_pri = nullptr;

    bool lastCheckLogicResult_pri = false;
    bool lastCheckLogicResultSet_pri = false;

    void clearExecutionResults_f() const;

    void deleteActionDataExecutionObject_f();
    void deleteActionDataExecutionResultObject_f();

    void deleteExecutionObjects_f();
    void deleteUsedPtrs_f();

    void execute_f();

    void prepareToExecuteAction_f();

    bool isKillingExecutionAfterTimeout_pri = false;

    void stopExecutingChecks_f();
    //must be called after tryStopExecution_f
    void kill_f();

    //bool isEditable_f() const;
    //use only on set value functions
    bool tryClearResultsOnEdit_f();

    virtual void derivedWrite_f(QJsonObject &json_ref_par) const = 0;
    virtual void derivedRead_f(const QJsonObject &json_par_con) = 0;
    //to be able to call isFieldsDataValid_f (from each derived class base "data" class, i.e. copyFileData_c::isFieldsDataValid_f) from action_c
    virtual bool derivedIsValidAction_f(textCompilation_c* errors_par = nullptr) const = 0;
    //virtual int_fast64_t derivedUpdateStringTriggerParserDependencies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con) = 0;

    virtual action_c* derivedClone_f() const = 0;

    //these might not be always required
    //return the number of updated items
    //see public versions to know what the arguments are
    virtual uint_fast64_t derivedUpdateActionStringIdDependencies_f(const QString& , const QString& );
    virtual uint_fast64_t derivedActionStringIdDependencyCount_f(const QString& ) const;
    virtual uint_fast64_t derivedStringTriggerCreationConflictCount_f(const QString& ) const;
    virtual uint_fast64_t derivedUpdateStringTriggerDependecies_f(const QString& , const QString& );
    virtual uint_fast64_t derivedStringTriggerDependencyCount_f(const QString& ) const;
    virtual QSet<QString> derivedStringTriggerCreationCollection_f() const;
    virtual QSet<QString> derivedStringTriggersInUse_f(const QSet<QString>& ) const;

    virtual baseActionExecution_c* createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par) = 0;
protected:
    explicit action_c();
    explicit action_c(
            const actionData_c& actionData_par_con
    );

    //delete the pointer variables if they aren't nullptr
    ~action_c();
public:


    void read_f(
            const QJsonObject &json_par_con
            //validates the check/s objects (and anything nested that can be validated)
            //and only loads the valid ones in the checkDataHub object
            , const bool loadOnlyValid_par_con
            //error text compilation to know why and which check objects aren't valid
            , textCompilation_c* errors_par = nullptr
    );
    void write_f(QJsonObject &json_ref_par) const;

    virtual actionType_ec type_f() const = 0;
    QString typeStr_f() const;
    //FUTURE dates (creation, modification with the option of hide/show in the grid)
    int_fast64_t id_f() const;
    QString description_f() const;
    bool checkResultLogicAnd_f() const;
    bool checksEnabled_f() const;
    bool enabled_f() const;
    //stops all execution if this action has an error
    bool stopAllExecutionOnError_f() const;
    bool repeatExecution_f() const;

    checksDataHub_c* checkDataHub_ptr_f();
    const checksDataHub_c& checkDataHub_f() const;

    bool isStoppingExecution_f() const;
    bool isExecuting_f() const;
    bool killingExecutionAfterTimeout_f() const;
    bool lastCheckLogicResult_f() const;
    bool lastCheckLogicResultSet_f() const;

    //returns number of updates, including this action action stringId update
    uint_fast64_t setStringId_f(
            const QString& stringId_par_con
            , const bool updateObjectsThatUsedOldValue_par_con = false
            , textCompilation_c* errorsPtr_par = nullptr);
    void setDescription_f(const QString& description_par_con);
    void setCheckResultLogicAnd_f(const bool checkResultLogicAnd_par_con);
    void setChecksEnabled_f(const bool checksEnabled_par_con);
    void setEnabled_f(const bool enabled_par_con);
    void setStopAllExecutionOnError_f(const bool stopAllExecutionOnError_par_con);
    void setRepeatExecution_f(const bool repeatExecution_par_con);

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
    uint_fast64_t updateActionStringIdDependencies_f(const QString& newActionStringId_par_con, const QString& oldActionStringId_par_con);
    uint_fast64_t actionStringIdDependencyCount_f(const QString& actionStringId_par_con) const;

    uint_fast64_t updateStringTriggerDependencies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con);
    //returns the ids of the first action-check that conflict, first = actionId, second = checkId, both id = 0 means no conflicts
    std::pair<int_fast64_t, int_fast64_t> stringTriggerCreationConflict_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par = nullptr) const;
    uint_fast64_t stringTriggerDependencyCount_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par = nullptr);

    //returns a collection of the stringTrigger than can be created dynamically on execution (20200128 right now only actionFinished does this)
    QSet<QString> stringTriggerCreationCollection_f() const;
    //pass the above function + stringParserMap_c::stringTriggers_f to get what's in use
    QSet<QString> stringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const;

    static action_c* readCreateDerived_f(const actionType_ec actionType_par_con);
    //aka the copy ctor alternative for polymorphic classes
    action_c* clone_f() const;

    bool isFieldsActionValid_f(textCompilation_c* errorsPtr_par = nullptr) const;

    //returns success, it can fail it the string id is already in use
    void updateActionData_f(const actionData_c& actionData_par_con
            , const bool updateObjectsThatUsedOldValue_par_con = false
            , textCompilation_c* errorsPtr_par = nullptr);

Q_SIGNALS:
    void actionStringIdChanged_signal(const QString& newActionStringId_par_con, const QString& oldActionStringId_par_con);
private Q_SLOTS:
    void setExecutionStateExecutingChecks_f();
    void examineCheckResults_f(const bool result_par_con);
    void setActionDataExecutionNull_f();
};

#endif // ACTONQTSO_ACTIONDATA_HPP
