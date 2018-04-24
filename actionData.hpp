//helper class/s to deal with the shortcomings of QTableWidget,
//because it only deals with rows and cells, it doesn't detect duplicates,
//it doesn't "map" where is anything, uses QVariant as underlying value,
//so... lets skip that, the Data and setData, and just use some extra mappings and have more functionality

#ifndef ACTONQTG_FILEDATA_HPP
#define ACTONQTG_FILEDATA_HPP

#include "mappings/actions.hpp"
#include "mappings/actionExecutionStates.hpp"

#include <QObject>
#include <QString>
#include <QJsonObject>

#include <string>
#include <unordered_map>

//FUTURE make description optional, for each action create a description function that can be used some generic description
//that use values of that specific action as a "description"
class actionData_c;
//this class it to deal in a generic way with all the action execution result/s
//so a createDirectory execution result/s can be displayed: display the directory path that has been created, or an error of why it couldn't
//but at the same runProcess execution result/s: return code, process output (stdout and stderr) and internal errors
//the results should be easily able to be show in console and in the gui version
//
class actionDataExecutionResult_c : public QObject
{
    Q_OBJECT
    //description of what the action did successfully during the execution
    QString output_pri;
    //if the action has any output (runProcess does, createDirectory doesn't)
    QString actionOutput_pri;
    //if the action has any error output (runProcess does, createDirectory doesn't)
    QString actionErrorOutput_pri;
    //description of any errors during the execution of this action
    QString errors_pri;
    //this might only apply to the runProcess kind, 0 should be success, anything else error,
    //BUT each process might vary
    //usually? it's a number from 0 to 255, https://en.wikipedia.org/wiki/Return_code
    int returnCode_pri = -1;
    bool returnCodeSet_pri = false;

    const actionData_c& parent_pri;

    //states
    //not running x not finished x no error (initial)
    //running x not finished x no error (running)
    //stopped x finished x no error (regular finish)
    //stopped x not finished x no error (user stopped)
    //stopped x not finished x error (error while running)
    //stopped x finished x error (possible?)
    actionExecutionState_ec actionState_pri = actionExecutionState_ec::initial;
    
    bool anyFinish_pri = false;

    qint64 startTime_pri = 0;
    bool startTimeSet_pri = false;
    qint64 finishedTime_pri = 0;
    bool finishedTimeSet_pri = false;

public:
    actionDataExecutionResult_c(
            const actionData_c& parentActionData_par_con
    );

    QString output_f() const;
    QString error_f() const;

    QString externalOutput_f() const;
    QString externalErrorOutput_f() const;

    int returnCode_f() const;
    bool returnCodeSet_f() const;

    actionExecutionState_ec actionState_f() const;
    const actionData_c& parent_f() const;

    bool anyFinish_f() const;

    qint64 startTime_f() const;
    bool startTimeSet_f() const;
    qint64 finishedTime_f() const;
    bool finishedTimeSet_f() const;

Q_SIGNALS:
    void outputUpdated_signal(const int_fast32_t actionDataId);
    void errorUpdated_signal(const int_fast32_t actionDataId);

    void externalOutputUpdated_signal(const int_fast32_t actionDataId);
    void externalErrorUpdated_signal(const int_fast32_t actionDataId);

    void returnCodeUpdated_signal(const int_fast32_t actionDataId);
    void executionStateUpdated_signal(const int_fast32_t actionDataId);

    void halt_signal();
    void anyFinish_signal(const int_fast32_t actionDataId);
public Q_SLOTS:
    void appendOutput_f(const QString& output_par_con);
    void appendError_f(const QString& error_par_con);

    void appendExternalOutput_f(const QString& actionOutput_par_con);
    void appendExternalError_f(const QString& actionError_par_con);
    //once set can't be changed
    void setReturnCode_f(const int returnCode_par_con);
    //some of the actionExecutionState are final, like finished, after the object can't be modified anymore
    //the other fields must be set first before transitioning into final state
    void setExecutionState_f(const actionExecutionState_ec actionExecutionState_par_con);

    void setAnyFinish_f();
};

//what goes in a row
class actionData_c
{
    //the id is a, fast, means to map the row position with the actionData
    int_fast64_t id_pri = 0;

    action_ec type_pri;
    QString description_pri;
    bool haltOnExecutionFail_pri = false;

    //FUTURE/OBSOLETE save execution output on a file, this can be manged outside using actionDataExecutionResult_c
    //bool saveExecutionOutputFile_pri = false;
    //QString saveExecutionOutputFilename_pri;

    QJsonObject actionDataJSON_pri;

    //it's a "plain" QObject because every execution object
    //has a different class but all have QObject as base and using the action type
    //it's possible to cast to the different execution classes
    QObject* executionObject_pri = Q_NULLPTR;
    actionDataExecutionResult_c* actionDataExecutionResult_pri = Q_NULLPTR;

    void deleteUsedPtrs_f();
public:
    explicit actionData_c();
    explicit actionData_c(
            const action_ec type_par_con
            , const QString& description_par_con
            , const bool haltOnFail_par_con
            , const QJsonObject& actionDataJson_par_con = QJsonObject()
    );
    //delete the pointer variables if they aren't Q_NULLPTR
    ~actionData_c();

    void write_f(QJsonObject &json) const;
    void read_f(const QJsonObject &json);

    //FUTURE dates (creation, modification poguent amagar i mostrar)
    int_fast64_t id_f() const;
    action_ec type_f() const;
    QString description_f() const;
    bool haltOnFail_f() const;
    QJsonObject actionDataJSON_f() const;

    void setType_f(const action_ec type_par_con);
    void setDescription_f(const QString& description_par_con);
    void setHaltOnFail_f(const bool haltOnFail_par_con);
    void setActionDataJSON_f(const QJsonObject& actionDataJSON_par_con);

    //actionDataExecutionResult_par must be "new" and have stuff connected to any of it's signals
    //because as soon as the execution starts slots might recieve signals
    //returns true if execution starts
    bool createExecution_f(actionDataExecutionResult_c* actionDataExecutionResult_par);
    actionDataExecutionResult_c* actionDataExecutionResultPtr_f() const;
};

//to de/serialize a file
class actionDataRoot_c
{
    std::vector<actionData_c> actionDataVector_pri;
public:
    actionDataRoot_c() = default;
    actionDataRoot_c(
            const std::vector<actionData_c>& actionDataVector_par_con);
   void write_f(QJsonObject &json) const;
   void read_f(const QJsonObject &json);

   std::vector<actionData_c> actionDataVector_f() const;
   //this will clear actionDataVector_pri
   void moveToDataHub_f();
};

//main hub of data
//in contrast to the fileData in hasherQtg this one can have duplicate elements (not taking into account the id)
class actionDataController_c //: public QObject
{
    //Q_OBJECT
    //it's necessary to have 2 because mapping is only one way, and some operations require access from actionDataId and others from row
    //key = actionData id, value = row number
    std::unordered_map<int_fast64_t, int> actionDataIdToRow_pri;
    //key = row number, value = actionData id
    std::unordered_map<int, int_fast64_t> rowToActionDataId_pri;

    //key = actionData Id, value = actionData obj
    std::unordered_map<int_fast64_t, actionData_c> actionDataIdToActionDataUMap_pri;

public:
    actionDataController_c() = default;
    //adds an object to the data "container", if the row exists it moves the target row to the next index (row+1)
    //cascading any row after to row+1
    //returns true if an insert happened, might not happen if the row value is invalid (negative or greater than the container size)
    bool insertActionData_f(const actionData_c& obj_par_con, const int row_par_con);
    //remove the object from the data "container" using actionId, if there is any row+1 moves in cascade all the following rows to row-1
    //returns true if a removal happened
    bool removeActionDataUsingId_f(const int_fast64_t actionDataId_par_con);
    //same but with row, optional pass actionDataId if it's known
    bool removeActionDataUsingRow_f(const int row_par_con, const int_fast64_t actionDataId_par_con = 0);

    bool moveRowActionData(const int sourceRow_par_con, const int destinationRow_par_con);

    //-1 if not found, >-1 otherwise
    int actionDataIdToRow_f(const int_fast64_t actionDataId_par_con) const;
    int_fast64_t rowToActionDataId_f(const int row_par_con) const;

    int_fast32_t size_f() const;

    std::pair<actionData_c&, bool> actionData_f(const int_fast64_t actionDataId_par_con);

    void clearAllActionData_f();

//    //both functions assume no dupes (id/row wise)
//    void runActionsDataId_f(const std::vector<int_fast32_t>& actionIds_par_con = std::vector<int_fast32_t>());
//    void runActionsFromRows_f(const std::vector<int>& rows_par_con = std::vector<int>());

    //ways to run actions
    //single action
    //selected actions
    //all actions

    //FUTURE
    //actions dependencies and concurrency
    //allow 2 executions models, sequential and dependency
    //sequential executes one action after the other and if an action fails in between it can or can't stop execution at that point
    //dependency model by default executes all actions at the same time, to solve that issue dependencies must be created, there can be cyclical dependencies
};

extern actionDataController_c& dataHub_f();

#endif // ACTONQTG_FILEDATA_HPP
