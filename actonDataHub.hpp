#ifndef ACTONQTSO_ACTONDATAHUB_HPP
#define ACTONQTSO_ACTONDATAHUB_HPP

#include "actionData.hpp"
#include "actonExecutionOptions.hpp"

#include "logsinJSONQtso/logDataHub.hpp"

#include "crossPlatformMacros.hpp"

#include <QObject>
#include <QHash>
#include <QStringList>

#include <unordered_map>
#include <vector>

//this macro should only be used inside this library: actonQtso
//don't use in actonDataHub_c ctors
#define MACRO_ADDACTONQTSOLOG(MESSAGE, TYPE) actonDataHub_ptr_ext->addLogMessage_f(MESSAGE, TYPE, MACRO_FILENAME, __func__, __LINE__)

//the 2 macro bellow are used in getter of string functions, the return is intended

//this macro will leave the original value if there is not parser configuration or no string "triggered"
#define COPYPARSERETURNVAR(X) QString copyStrTmp(X); \
if (actonDataHub_ptr_ext not_eq nullptr and actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f() not_eq nullptr) \
{ \
    actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->executeForString_f(std::addressof(copyStrTmp)); \
} \
return copyStrTmp;

//same as above but for container of QString
#define COPYPARSERETURNSTRINGLIST(X) QStringList copyStrListTmp(X); \
if (actonDataHub_ptr_ext not_eq nullptr and actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f() not_eq nullptr) \
{ \
    for (QString& str_ite : copyStrListTmp) \
    { \
        actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->executeForString_f(std::addressof(str_ite)); \
    } \
} \
return copyStrListTmp;

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

    bool stopWhenCurrentExecutionCycleFinishes_pri = false;

    uint_fast64_t executionLoopCount_pri = 0;

    void executeActions_f(const bool loop_par_con = false);

public:
    explicit actonDataHub_c(QObject* parent_par);

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
    void executeActionDataRows_f(std::vector<int> rows_par = std::vector<int>());
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
    QString actionStringIdExecutingChecks_f() const;
    bool stoppingActionsExecution_f() const;
    bool actionsExecutionStopped_f() const;
    bool actionsExecutionFinished_f() const;

//public Q_SLOTS:
    //FUTURE add try to stop for a selection of actions
    void tryStopExecutingActions_f(const bool killAfterTimeout_par_con = false);
    //because otherwise when executing loops the only way to stop them is using tryStopExecutingActions_f
    //and that leaves some actions finished, some executing-stopped and some not executed at all
    //this will stop at the end of the overall execution cycle
    //it will also stop (once their finish their current execution) actions not bound by a "execution cycle"
    //will stop keepExecuting_pro = true actions too
    void stopWhenCurrentExecutionCycleFinishes_f();

    bool hasStringIdAnyDependency_f(const QString& stringId_par_con) const;

    //objectToIgnore_par can be nullptr, this argument is to ignore the current
    //object being edited when checking for string trigger dependencies
    bool hasStringTriggerAnyDependency_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par) const;
    //although the return value is a vector, it will only contain unique strings
    std::vector<QString> stringTriggersInUseByActionsOrChecks_f() const;

    bool killingActionsExecution_f() const;
    bool actionsExecutionKilled_f() const;

    bool logDataHubSet_f() const;
    //optional, if properly set the class will log changes
    void setLogDataHub_f(logDataHub_c* logDataHub_par);
    //will do nothing if logDataHub_pri hasn't been set
    bool addLogMessage_f(
            const text_c& message_par_con
            , const logItem_c::type_ec logType_par_con
            , const QString& sourceFile_par_con
            , const QString& sourceFunction_par_con
            , const int_fast32_t line_par_con
    );

    //verifies if ALL the actions are valid
    //a more optimized version of this could be made, i.e. get the first invalid one...
    //but right now this function is used when deserializing and before execution where an ALL version is required
    bool areActionsValid_f(textCompilation_c* errors_par = nullptr) const;
Q_SIGNALS:
    void actionsExecutionStarted_signal();
    void stoppingActionsExecution_signal();
    void actionsExecutionStopped_signal();
    void killingActionsExecution_signal();
    void actionsExecutionKilled_signal();
    void actionsExecutionFinished_signal(std::vector<action_c*> lastRunActions_par);
    void anyExecutingChecksStopped_signal();
private Q_SLOTS:
    void verifyExecutionFinished_f(action_c* actionPtr_par);
    void killingStarted_f();
public Q_SLOTS:
    //update all the actions/checks setting that depend on an actionStringId
    //returns the number of updated checks/action properties which did match with the oldStringId
    int_fast32_t updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con);
    int_fast32_t updateStringTriggerDependencies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con);
};

extern EXPIMP_ACTONQTSO actonDataHub_c* actonDataHub_ptr_ext;

#endif // ACTONQTSO_ACTONDATAHUB_HPP
