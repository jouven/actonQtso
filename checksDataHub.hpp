#ifndef ACTONQTSO_CHECKSDATAHUB_HPP
#define ACTONQTSO_CHECKSDATAHUB_HPP

#include "crossPlatformMacros.hpp"

#include <QObject>

#include <unordered_map>
#include <vector>
#include <deque>
//#include <memory>

class action_c;
class check_c;

class EXPIMP_ACTONQTSO checksDataHubProxyQObj_c : public QObject
{
    Q_OBJECT
public:
Q_SIGNALS:
    void checksExecutionStarted_signal();
    void stoppingChecksExecution_signal();
    void checksExecutionStopped_signal();
    void checksExecutionFinished_signal(std::vector<check_c*> lastRunChecks_par);
};

class EXPIMP_ACTONQTSO checksDataHub_c //: public QObject
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
    std::unordered_map<int_fast64_t, check_c*> checkDataIdToCheckUMap_pri;

    action_c* parentAction_pri = nullptr;

    checksDataHubProxyQObj_c* proxyQObj_pri = nullptr;

    std::vector<check_c*> checksToRun_pri;
    //checks to run sequentially
    std::deque<check_c*> checksToRunSeq_pri;

    bool executingChecks_pri = false;
    bool checksExecutionFinished_pri = false;
    bool stoppingChecksExecution_pri = false;
    bool checksExecutionStopped_pri = false;

    void executeChecks_f();
    void verifyExecutionFinished_f();
    void executeNextSeqCheck_f();
public:
    explicit checksDataHub_c(action_c* parentAction_par);

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

    void setParentAction_f(action_c* parentAction_par);

    ~checksDataHub_c();

    //to check if a row value can be inserted, not negative and row/index=<"container size"
    bool validRow_f(const int row_par_con) const;

    //adds an object to the data "container", if the row exists it moves the target row to the next index (row+1)
    //cascading any row after to row+1
    //returns true if an insert happened, might not happen if the row value is invalid (negative or greater than the container size)
    //or the actionString Id is already on use or empty
    bool insertCheck_f(check_c* obj_par, const int row_par_con);
    //remove the object from the data "container" using actionId, if there is any row+1 moves in cascade all the following rows to row-1
    //returns true if a removal happened
    bool removeCheckUsingId_f(const int_fast64_t checkDataId_par_con);
    //same but with row, optional pass actionDataId if it's known
    bool removeCheckUsingRow_f(const int row_par_con, const int_fast64_t checkDataId_par_con = 0);

    bool moveCheckRow_f(const int sourceRow_par_con, const int destinationRow_par_con);

    //-1 if not found, >-1 otherwise
    int checkDataIdToRow_f(const int_fast64_t checkDataId_par_con) const;
    int_fast64_t rowToCheckDataId_f(const int row_par_con) const;

    int_fast32_t size_f() const;

    check_c* check_ptr_f(const int_fast64_t checkDataId_par_con);
    check_c* check_ptr_f(const int_fast64_t checkDataId_par_con) const;
    //checkData_c checkData_f(const int_fast64_t checkDataId_par_con, bool* found_ptr = nullptr) const;

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
    action_c* parentAction_f() const;
    //update all the checks setting that depend on an actionStringId
    //returns the number of updated checks which did match with the oldStringId
    int_fast32_t updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con);
    bool hasStringIdAnyDependency_f(const QString& stringId_par_con) const;

    int_fast32_t updateStringTriggerParserDependencies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con);
    bool hasStringTriggerAnyDependency_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par) const;
    //although the return value is a vector, it will only contain unique strings
    std::vector<QString> stringTriggersInUseByChecks_f() const;
};

#endif // ACTONQTSO_CHECKSDATAHUB_HPP
