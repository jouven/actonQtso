#ifndef ACTONQTSO_CHECKSDATAHUB_HPP
#define ACTONQTSO_CHECKSDATAHUB_HPP

#include "checkData.hpp"

#include <QObject>

#include <unordered_map>
#include <vector>
#include <deque>
//#include <memory>

class actionData_c;

class checksDataHubProxyQObj_c : public QObject
{
    Q_OBJECT
public:
Q_SIGNALS:
    void checksExecutionStarted_signal();
    void stoppingChecksExecution_signal();
    void checksExecutionStopped_signal();
    void checksExecutionFinished_signal(std::vector<checkData_c*> lastRunChecks_par);
};

class checksDataHub_c //: public QObject
{
    //Q_OBJECT

    //"row" is defines the position "order" of the actions when visually represented,
    //used on actonQtg to show the actions in order in a grid,
    //used when saving.
    //Rigth now defines execution order too, but this will change in the future

    //it's necessary to have 2 because mapping is only one way, and some operations require access from actionDataId and others from row
    //key = checkData id, value = row number
    std::unordered_map<int_fast64_t, int> checkDataIdToRow_pri;
    //key = row number, value = checkData id
    std::unordered_map<int, int_fast64_t> rowToCheckDataId_pri;

    //key = checkData Id, value = checkData obj
    std::unordered_map<int_fast64_t, checkData_c> checkDataIdToCheckDataUMap_pri;

    actionData_c* parentAction_pri = nullptr;

    checksDataHubProxyQObj_c* proxyQObj_pri = nullptr;

    std::vector<checkData_c*> checksToRun_pri;
    //checks to run sequentially
    std::deque<checkData_c*> checksToRunSeq_pri;
    //when the checks of an action finish executing, the action
    //needs to examine their results to decide if it can run
    //since action_c is no QObject it's necessary to put
    //put a property so the action can access them
    std::vector<checkData_c*> lastRunChecks_pri;

    bool executingChecks_pri = false;
    bool checksExecutionFinished_pri = false;
    bool stoppingChecksExecution_pri = false;

    void executeChecks_f();
    void verifyExecutionFinished_f();
    void executeNextSeqCheck_f();
public:
    explicit checksDataHub_c(actionData_c* parentAction_par);

    //copies everything except proxy pointer
    checksDataHub_c(const checksDataHub_c& from_par_con);
    //needed to allow parent action object to set a new parent of this object when copying an action,
    //does the same as the above copy ctor
    checksDataHub_c(checksDataHub_c& from_par);
    //std::moves everything and nulls from_par pointers
    checksDataHub_c(checksDataHub_c&& from_par) noexcept;
    //copies everything except pointer proxy, deletes proxy obj
    checksDataHub_c& operator= (const checksDataHub_c& from_par_con);
    //same as above
    checksDataHub_c& operator=(checksDataHub_c& from_par);
    //same as move ctor
    checksDataHub_c& operator=(checksDataHub_c&& from_par) noexcept;

    //when copying/asign/move an action, the "new" action calls this
    void setParentAction_f(actionData_c* parentAction_par);

    ~checksDataHub_c();

    //to check if a row value can be inserted, not negative and row/index=<"container size"
    bool validRow_f(const int row_par_con) const;

    //adds an object to the data "container", if the row exists it moves the target row to the next index (row+1)
    //cascading any row after to row+1
    //returns true if an insert happened, might not happen if the row value is invalid (negative or greater than the container size)
    //or the actionString Id is already on use or empty
    bool insertCheckData_f(const checkData_c& obj_par, const int row_par_con);
    //remove the object from the data "container" using actionId, if there is any row+1 moves in cascade all the following rows to row-1
    //returns true if a removal happened
    bool removeCheckDataUsingId_f(const int_fast64_t checkDataId_par_con);
    //same but with row, optional pass actionDataId if it's known
    bool removeCheckDataUsingRow_f(const int row_par_con, const int_fast64_t checkDataId_par_con = 0);

    bool moveRowCheckData_f(const int sourceRow_par_con, const int destinationRow_par_con);

    //-1 if not found, >-1 otherwise
    int checkDataIdToRow_f(const int_fast64_t checkDataId_par_con) const;
    int_fast64_t rowToCheckDataId_f(const int row_par_con) const;

    int_fast32_t size_f() const;

    checkData_c* checkData_ptr_f(const int_fast64_t checkDataId_par_con);
    checkData_c checkData_f(const int_fast64_t checkDataId_par_con, bool* found_ptr = nullptr) const;

    //not in use
    //void clearAllCheckData_f();
    //does the signals of checksDataHub_c
    //this way there is no need to make checksDataHub_c a QObject derived class
    checksDataHubProxyQObj_c* proxyQObj_f();

    //if empty it will execute all the checks
    void executeCheckDataRows_f(std::vector<int> rows_par = std::vector<int>());

    bool executingChecks_f() const;
    bool stoppingChecksExecution_f() const;
    bool checksExecutionFinished_f() const;

    void stopExecutingChecks_f();
    std::vector<checkData_c*> lastRunChecks_f() const;
    actionData_c* parentAction_f() const;
    //update all the checks setting that depend on an actionStringId
    //returns the number of updated checks which did match with the oldStringId
    int_fast32_t updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con);
    bool hasStringIdAnyDependency_f(const QString& stringId_par_con) const;
};

#endif // ACTONQTSO_CHECKSDATAHUB_HPP
