#ifndef ACTONQTSO_ACTONDATAHUB_HPP
#define ACTONQTSO_ACTONDATAHUB_HPP

#include "actionData.hpp"
#include "actonExecutionOptions.hpp"

#include "logsinJSONQtso/logDataHub.hpp"

#include "crossPlatformMacros.hpp"

#include <QObject>
#include <QHash>
#include <QStringList>

#include <deque>
#include <unordered_map>
#include <QSet>
#include <vector>

//this macro should only be used inside this library: actonQtso
#define MACRO_FILEMETA MACRO_FILENAME, __func__, __LINE__
#define MACRO_ADDACTONDATAHUBLOG(X,...) X->addLogMessage_f(__VA_ARGS__, MACRO_FILEMETA)

class executionMessage_c;
class executionResult_c;

//FUTURE (FAR?) action result's to file, actions can use this files as a source
//make result clases de/serializable
//i.e. create directory can grab, runprocess "ls" result and try to create folders (I know it doesn't make too much sense)
//make actions, generically able to use output of other actions, actions should "create" usable output
//i.e. directory creation should output the directory path if successful/exists
//also create a "find" action that makes lists of files/directories (see findFilesDirectories in the enum)
//option to enable/disable output in an action, to be ablet save it on a file
//In conclusion:
//1 create "usable" action output
//2 allow to save this output on a json file, make result clases de/serializable
//3 allow to use the result clases or the result files of point 2 as source for actions
//


//main hub of data
class EXPIMP_ACTONQTSO actonDataHub_c : public QObject
{
    Q_OBJECT
    //data variables

    //"row" is defines the position "order" of the actions when visually represented,
    //used on actonQtg to show the actions in order in a grid,
    //used when saving.
    //Rigth now defines execution order too, but this will change in the future

    //it's necessary to have 2 because mapping is only one way, and some operations require access from actionDataId and others from row
    //key = actionData id, value = row number
    std::unordered_map<int_fast64_t, int> actionDataIdToRow_pri;
    //key = row number, value = actionData id
    std::unordered_map<int, int_fast64_t> rowToActionDataId_pri;

    //key = actionData stringId, value = actionData Id
    QHash<QString, int_fast64_t> actionDataStringIdToActionDataId_pri;
    //key = actionData Id, value = actionData obj
    std::unordered_map<int_fast64_t, action_c*> actionDataIdToActionPtrUMap_pri;

    executionOptions_c executionOptions_pri;
    //bool executingActions_pri = false;
    std::vector<action_c*> actionsToRun_pri;

    logDataHub_c* logDataHub_pri = nullptr;

    //control variables
    bool executingActions_pri = false;
    bool actionsExecutionFinished_pri = false;
    bool stoppingActionsExecution_pri = false;
    bool actionsExecutionStopped_pri = false;
    bool killingActionsExecution_pri = false;
    bool actionsExecutionKilled_pri = false;
    bool resumingExecution_pri = false;

    bool stopWhenCurrentExecutionCycleFinishes_pri = false;

    uint_fast64_t executionLoopCount_pri = 0;

    void executeActions_f(const bool loop_par_con = false);

    std::deque<executionMessage_c*> executionMessages_pri;
    std::deque<executionResult_c*> executionResults_pri;
    uint_fast64_t executionMessageMaxDequeSize_pri = 100000;
    uint_fast64_t executionResultMaxDequeSize_pri = 100000;
    //when the cache is full remove the first/oldest executionMessageClearCacheAmount_pri items from the cache
    uint_fast64_t executionMessageClearCacheAmount_pri = executionMessageMaxDequeSize_pri / 10;
    //when the cache is full remove the first/oldest executionResultMaxQueueSize_pri items from the cache
    uint_fast64_t executionResultClearCacheAmount_pri = executionResultMaxDequeSize_pri / 10;

    void addExecutionMessage_f(executionMessage_c* executionMessage_par);

public:
    //can't use const ptr argument because QObject* ctor argument is non-const
    explicit actonDataHub_c(QObject* parent_par);
    ~actonDataHub_c();

    //to check if a row value can be inserted, not negative and index=<"container size"
    bool validRow_f(const int row_par_con) const;
    //to check if a actionDataStringId can be inserted, must be unique and not empty
    bool validStringId_f(const QString& actionDataStringId_par_con) const;

    //adds an object to the data "container", if the row exists it moves the target row to the next index (row+1)
    //cascading any row after to row+1
    //returns true if an insert happened, might not happen if the row value is invalid (negative or greater than the container size)
    //or the actionStringId is already in use or empty
    bool insertActionData_f(action_c* const actionPtr_par, const int row_par_con, textCompilation_c* errorsPtr_par = nullptr);
    //remove the object ptr from the data "container" using actionId, if there is any row+1 moves in cascade all the following rows to row-1
    //returns true if a removal happened (does not remove or delete the actual object)
    bool removeActionDataUsingId_f(const int_fast64_t actionDataId_par_con);
    //same as above but with row, optional pass actionDataId if it's known
    bool removeActionDataUsingRow_f(const int row_par_con, const int_fast64_t actionDataId_par_con = 0);

    //moves a row to the position of another row, DOESN'T REPLACE,
    //or the last position if the destination row = container size
    //it will cascade move other rows down or up if needed
    bool moveRowActionData_f(const int sourceRow_par_con, const int destinationRow_par_con);

    //-1 if not found, >-1 otherwise
    int actionDataIdToRow_f(const int_fast64_t actionDataId_par_con) const;
    QString actionDataIdToActionDataStringId_f(const int_fast64_t actionDataId_par_con) const;
    int_fast64_t rowToActionDataId_f(const int row_par_con) const;
    int_fast64_t actionDataStringIdToActionDataId_f(const QString& actionDataStringId_par_con) const;

    int_fast32_t size_f() const;

    action_c* action_ptr_f(const int_fast64_t actionDataId_par_con);
    action_c* action_ptr_f(const int_fast64_t actionDataId_par_con) const;
    //actionData_c actionData_f(const int_fast64_t actionDataId_par_con, bool* found_ptr = nullptr) const;

    void clearAllActionData_f();

    //return all the actionStringIds from the actionData objects
    //stored in this class (actionDataIdToActionDataUMap_pri),
    //in a list
    //used in the lastActionFinishedCheck combo (actonQtg)
    QStringList actionStringIdList_f(const bool sorted_par_con = true) const;

    executionOptions_c executionOptions_f() const;
    void setExecutionOptions_f(const executionOptions_c& executionOptions_par_con);
    //executionOptions_c& executionOptionsRef_f();

    //actions

    //if empty it will execute all the actions
    //will remove dupes if any
    //rows are used instead of actionId because of "order"
    //actionIds could be used too, but, rows are ordered
    //this way having to check if the actionIds are in order or getting their order can be skipped
    //and even if rows weren't in order, they'll be after the remove dupes step
    bool executeActionDataRows_f(std::vector<int> rows_par = std::vector<int>(), textCompilation_c* errors_par = nullptr);
    void tryResumeActionsExecution_f();

    bool executingActions_f() const;

    //this checks every action checksHub executingChecks_f
    //checks executing will prevent executeActionDataRows_f
    //bool isAnyActionExecutingChecks_f() const;
    //void stopAnyExecutingChecks_f();
    //complementary function for the above one
    //so the user can be hinted to which action has check/s executing
    //in theory multiple checks of diferent action could be executing at the same time
    //since they won't conflict, but in actonQtg this won't be allowed
    //QString actionStringIdExecutingChecks_f() const;
    bool stoppingActionsExecution_f() const;
    bool actionsExecutionStopped_f() const;
    bool actionsExecutionFinished_f() const;


    //FUTURE add try to stop for a selection of actions
    //TODOIMPORTANT about threading and stopping action execution
    //what happens when a stop occurs just when an action, that uses a thread for execution, finishes executing?:
    //1 that the threaded code encounters a "stop check" and the action execution stops
    //2 that it has "no effect" on the execution because no "stop check" is encountered, this has two "sub-ends"
    //2a the action execution has finished doing w/e was doing but when it finishes it ends "stopped" because the previous/last state was stopping
    //2b finishes in success/error (if an error happened) because
    //the action finished before the state change to stopping happened (really small window of time for this to happen)

    //the IMPORTANT fact here is: action stopping code and action finishing-deletion code execute in the main thread,
    //they can't happen at the same time

    void tryStopExecutingActions_f(const bool killAfterTimeout_par_con = false);
    //because otherwise when executing loops the only way to stop them is using tryStopExecutingActions_f
    //and that leaves some actions finished, some executing-stopped and some not executed at all
    //this will stop at the end of the overall execution cycle
    //it will also stop (once their finish their current execution) actions not bound by a "execution cycle"
    //will stop keepExecuting_pro = true actions too
    void stopWhenCurrentExecutionCycleFinishes_f();

    //returns the amount of dependencies found for the stringId (it might not be in use by any action)
    uint_fast64_t actionStringIdDependencyCount_f(const QString& actionStringId_par_con) const;

    //objectToIgnore_par can be nullptr, this argument is to ignore a possible
    //object (action/check pointer as of 20200122) being edited when checking for string trigger creation conflicts
    //A conflict can happen when more than one object create the same stringTrigger (one will try to replace the other and will error when trying to)
    //return value possible values: ids > 0, action/check conflict (first = actionId, second = checkId) | ids = 0, stringParserMap conflict | ids = -1, no ids conflict
    std::pair<int_fast64_t, int_fast64_t> stringTriggerCreationConflict_f(
            const QString& stringTrigger_par_con
            , const void* const objectToIgnore_par = nullptr
            //this only affects if a stringParserMap is set in the executing options, otherwise it will be ignored
            , const bool includeStringParserMap_par_con = true) const;
    //returns the number of references for a stringTrigger (the stringTrigger might not exist/be created anywhere)
    uint_fast64_t stringTriggerDependencyCount_f(const QString& stringId_par_con, const void* const objectToIgnore_par = nullptr) const;

    //returns a collection of the stringTrigger than can be created dynamically on execution (20200128 right now only actionFinished does this)
    QSet<QString> stringTriggerCreationCollection_f() const;
    //same as stringTriggerDependencyCount_f but returning the stringTrigger/s in use
    QSet<QString> stringTriggersInUseByActionsOrChecks_f() const;

    bool killingActionsExecution_f() const;
    bool actionsExecutionKilled_f() const;

    bool logDataHubSet_f() const;
    //optional, if properly set the class will log changes
    void setLogDataHub_f(logDataHub_c* logDataHub_par);
    //will do nothing if logDataHub_pri hasn't been set
    bool addLogMessage_f(
            const text_c& message_par_con
            , const messageType_ec logType_par_con
            , const QString& sourceFile_par_con
            , const QString& sourceFunction_par_con
            , const int_fast32_t line_par_con
    ) const;
    bool addLogMessage_f(
            const text_c& message_par_con
            , const action_c* actionPtr_par
            , const messageType_ec  logType_par_con
            , const QString& sourceFile_par_con
            , const QString& sourceFunction_par_con
            , const int_fast32_t line_par_con
    ) const;
    bool addLogMessage_f(
            const text_c& message_par_con
            , const check_c* checkPtr_par
            , const messageType_ec logType_par_con
            , const QString& sourceFile_par_con
            , const QString& sourceFunction_par_con
            , const int_fast32_t line_par_con
    ) const;

    //verifies if ALL the actions are valid
    //a more optimized version of this could be made, i.e. get the first invalid one...
    //but right now this function is used when deserializing and before execution where an ALL version is required
    bool areActionsValid_f(textCompilation_c* errors_par = nullptr) const;

    //this is for the special actions/checks types that depend on other actions/checks, action/check regular validation can't/won't check
    //if other actions/checks are in the state they require.
    //Check for actionFinished, actionStartedExecuting and folderChangeReaction if their dependencies are met
    //i.e., actionFinished action should be enabled, same with actionStartedExecuting, on the other hand folderChangeReaction actions should be disabled
    //but valid (which they aren't checked for during execution)
    bool areCheckActionDependenciesValid_f(textCompilation_c* errorsPtr_par = nullptr) const;

    std::deque<executionMessage_c*> executionMessages_f() const;
    std::deque<executionResult_c*> executionResults_f() const;

Q_SIGNALS:
    void actionsExecutionStarted_signal();
    void stoppingActionsExecution_signal();
    void actionsExecutionStopped_signal();
    void killingActionsExecution_signal();
    void actionsExecutionKilled_signal();
    void actionsExecutionFinished_signal(std::vector<action_c*> lastRunActions_par);
    void executionMessageAdded_signal(const executionMessage_c* message_par);
    void executionResultAdded_signal(const executionResult_c* executionResult_par);
    //void anyExecutingChecksStopped_signal();
private Q_SLOTS:
    void verifyExecutionFinished_f(executionResult_c* executionResultPtr_par);
    void killingStarted_f();
public Q_SLOTS:
    //update all the actions/checks fields that reference/depend on an actionStringId (not the actionStringId field of an action object)
    //returns the number of updated checks/action properties which did match with the oldStringId
    //this call should be optional when changing an action of this actonDataHub_c with a different actionStringId
    uint_fast64_t updateStringIdDependencies_f(const QString& newActionStringId_par_con, const QString& oldActionStringId_par_con);
    //update the string fields (those that have a "parsed" variant) that use the string trigger in all the actions/checks in the actonDataHuc_c object
    uint_fast64_t updateStringTriggerDependencies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con);
    //doesn't modify any action it only updates actionDataStringIdToActionDataId_pri
    //it's automatically called when an action of this actonDataHub_c is updated with a different actionStringId
    bool changeActionStringId_f(const QString& oldActionStringId_par_con, const QString& newActionStringId_par_con);

    //this is used for checks too
    void addExecutionResult_f(executionResult_c* executionResult_par);
    void addActionExecutionResultMessage_f(executionResult_c*, executionMessage_c* message_par);
    void addCheckExecutionResultMessage_f(executionResult_c*, executionMessage_c* message_par);
};

//extern EXPIMP_ACTONQTSO actonDataHub_c* actonDataHub_ptr_ext;

#endif // ACTONQTSO_ACTONDATAHUB_HPP
