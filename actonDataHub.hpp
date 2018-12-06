#ifndef ACTONQTSO_ACTONDATAHUB_HPP
#define ACTONQTSO_ACTONDATAHUB_HPP

#include "actionData.hpp"
#include "actonExecutionOptions.hpp"

#include "logsinJSONQtso/logDataHub.hpp"

#include <QObject>
#include <QHash>
#include <QStringList>

#include <unordered_map>
#include <vector>

//this macro should only be used inside this library: actonQtso
//don't use in actonDataHub_c ctors
#define MACRO_ADDACTONQTSOLOG(MESSAGE, TYPE) actonDataHub_ptr_ext->addLogMessage_f(MESSAGE, TYPE, MACRO_FILENAME, __func__, __LINE__)

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

class actonDataHubProxyQObj_c : public QObject
{
    Q_OBJECT
public:
    actonDataHubProxyQObj_c();
Q_SIGNALS:
    void actionsExecutionStarted_signal();
    void stoppingActionsExecution_signal();
    void actionsExecutionStopped_signal();
    void killingActionsExecution_signal();
    void actionsExecutionKilled_signal();
    void actionsExecutionFinished_signal(std::vector<actionData_c*> lastRunActions_par);
    void anyExecutingChecksStopped_signal();
};

class logDataHub_c;

//main hub of data
class actonDataHub_c
{
    actonDataHubProxyQObj_c* const proxyQObj_pri;
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
    std::unordered_map<int_fast64_t, actionData_c> actionDataIdToActionDataUMap_pri;

    executionOptions_c executionOptions_pri;
    //bool executingActions_pri = false;
    std::vector<actionData_c*> actionsToRun_pri;

    logDataHub_c* logDataHub_pri = nullptr;

    bool executingActions_pri = false;
    bool actionsExecutionFinished_pri = false;
    bool stoppingActionsExecution_pri = false;
    bool actionsExecutionStopped_pri = false;
    bool killingActionsExecution_pri = false;
    bool actionsExecutionKilled_pri = false;
    //TODO implement
    //when looping execution, right now stopping will stop leaving actions midway execution
    //or actions that were waiting for others will just not execute at all
    //create an option to stop at the end of the loop
    bool stopOnThisLoopEnd_pri = false;

    void executeActions_f(const bool loop_par_con = false);
    void verifyExecutionFinished_f();
    void killingStarted_f();
public:
    explicit actonDataHub_c();

    //to check if a row value can be inserted, not negative and index=<"container size"
    bool validRow_f(const int row_par_con) const;
    //to check if a actionDataStringId can be inserted, must be unique and not empty
    bool validStringId_f(const QString& actionDataStringId_par_con) const;

    //adds an object to the data "container", if the row exists it moves the target row to the next index (row+1)
    //cascading any row after to row+1
    //returns true if an insert happened, might not happen if the row value is invalid (negative or greater than the container size)
    //or the actionString Id is already in use or empty
    bool insertActionData_f(const actionData_c& obj_par_con, const int row_par_con);
    //remove the object from the data "container" using actionId, if there is any row+1 moves in cascade all the following rows to row-1
    //returns true if a removal happened
    bool removeActionDataUsingId_f(const int_fast64_t actionDataId_par_con);
    //same but with row, optional pass actionDataId if it's known
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

    actionData_c* actionData_ptr_f(const int_fast64_t actionDataId_par_con);
    //actionData_c actionData_f(const int_fast64_t actionDataId_par_con, bool* found_ptr = nullptr) const;

    void clearAllActionData_f();

    //return all the actionStringIds from the actionData objects
    //stored in this class (actionDataIdToActionDataUMap_pri),
    //in a list
    //used in the lastActionFinishedCheck combo (actonQtg)
    QStringList actionStringIdList_f(const bool sorted_par_con = true) const;

    //no reference function "option" because this class needs to know when it's modified, this way it knows
    void setExecutionOptions(const executionOptions_c& executionOptions_par_con);
    executionOptions_c executionOptions_f() const;

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
//private Q_SLOTS:
    //void checkLoopExecution_f();
    //ways to run actions
    //single action
    //selected actions
    //all actions

    //does the signals of actonDataHub_c
    //this way there is no need to make actonDataHub_c a QObject derived class
    actonDataHubProxyQObj_c* proxyQObj_f() const;

    //update all the actions/checks setting that depend on an actionStringId
    //returns the number of updated checks/action properties which did match with the oldStringId
    int_fast32_t updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con);
    bool hasStringIdAnyDependency_f(const QString& stringId_par_con) const;

    bool killingActionsExecution_f() const;
    bool actionsExecutionKilled_f() const;

    bool logDataHubSet_f() const;
    //optional, if properly set the class will log changes
    void setLogDataHub_f(logDataHub_c* logDataHub_par);
    //will do nothing if logDataHub_pri hasn't been set
    bool addLogMessage_f(
            const QString& message_par_con
            , const logItem_c::type_ec logType_par_con
            , const QString& sourceFile_par_con
            , const QString& sourceFunction_par_con
            , const int_fast32_t line_par_con
    );
};

extern actonDataHub_c* actonDataHub_ptr_ext;

#endif // ACTONQTSO_ACTONDATAHUB_HPP
