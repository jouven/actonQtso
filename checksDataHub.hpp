#ifndef ACTONQTSO_CHECKSDATAHUB_HPP
#define ACTONQTSO_CHECKSDATAHUB_HPP

#include "crossPlatformMacros.hpp"

#include <QObject>
#include <QSet>

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <deque>
#include <QObject>
//#include <memory>

class action_c;
class check_c;
class textCompilation_c;
class executionMessage_c;
class executionResult_c;


//20191124 parentActionPtrTmp->checkResultLogicAnd_f() makes this class hard depend on having a parent set to an action_c object
//then again this class is always-only used in action_c
//"it defines the checks of an action"
class EXPIMP_ACTONQTSO checksDataHub_c : public QObject
{
    Q_OBJECT

    //data variables

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

    typedef decltype(checkDataIdToRow_pri.size()) row_t;

    //control variables
    std::vector<check_c*> checksToRun_pri;
    //checks to run sequentially
    std::deque<check_c*> checksToRunSeq_pri;

    bool executingChecks_pri = false;
    bool stoppingChecksExecution_pri = false;
    bool checksExecutionStopped_pri = false;

    void executeChecks_f();

public:
    explicit checksDataHub_c(action_c* parentAction_par);

    //DON'T use the regular constructors to explicitly copy a checksDataHub_c object, use clone (because pointers)

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

    //set this checkDataHub parent action and sets the parents of all the checks objects
    //use this instead of the QObject setParent
    void setParentAction_f(QObject* parentAction_par);

    //~checksDataHub_c();

    //to check if a row value can be inserted, not negative and row/index=<"container size"
    bool validRow_f(const row_t row_par_con) const;

    //adds an object to the data "container", if the row exists it moves the target row to the next index (row+1)
    //cascading any row after to row+1
    //returns true if an insert happened, might not happen if the row value is invalid (negative or greater than the container size)
    //or the action stringd is already in use or empty
    bool insertCheck_f(check_c* obj_par, const row_t row_par_con);
    //remove the object pointer from the data "container" using actionId, if there is any row+1 moves in cascade all the following rows to row-1
    //returns true if a removal happened (does not remove or delete the actual object)
    bool removeCheckUsingId_f(const int_fast64_t checkDataId_par_con);
    //same but with row, optional pass actionDataId if it's known
    bool removeCheckUsingRow_f(const row_t row_par_con, const int_fast64_t checkDataId_par_con = 0);

    bool moveCheckRow_f(const int sourceRow_par_con, const int destinationRow_par_con);

    //-1 if not found, >-1 otherwise
    int checkDataIdToRow_f(const int_fast64_t checkDataId_par_con) const;
    int_fast64_t rowToCheckDataId_f(const int row_par_con) const;

    decltype(rowToCheckDataId_pri.size()) size_f() const;

    check_c* check_ptr_f(const int_fast64_t checkDataId_par_con);
    check_c* check_ptr_f(const int_fast64_t checkDataId_par_con) const;
    //checkData_c checkData_f(const int_fast64_t checkDataId_par_con, bool* found_ptr = nullptr) const;

    //not in use
    //void clearAllCheckData_f();

    //if empty it will execute all the checks
    //returns the checks that will be executed (enabled, valid, not already running)
    std::vector<check_c*> executeCheckDataRows_f(std::vector<int> rows_par = std::vector<int>());

    bool executingChecks_f() const;
    bool stoppingChecksExecution_f() const;

    void stopExecutingChecks_f();
    bool checksExecutionStopped_f() const;
    //action_c* parentAction_f() const;
    //update all the checks setting that depend on an actionStringId
    //returns the number of updated checks which did match with the oldStringId
    uint_fast64_t updateActionStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con);
    uint_fast64_t actionStringIdDependencyCount_f(const QString& stringId_par_con) const;

    //return the first check id that conflicts, 0 = no conflict
    int_fast64_t stringTriggerCreationConflict_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par = nullptr) const;
    uint_fast64_t updateStringTriggerDependencies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con);
    uint_fast64_t stringTriggerDependencyCount_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par = nullptr) const;

    QSet<QString> stringTriggerCreationCollection_f() const;
    QSet<QString> stringTriggersInUseByChecks_f(const QSet<QString>& searchValues_par_con) const;

    //verifies if ALL the checks are valid
    //a more optimized version of this could be made, i.e. get the first invalid one...
    //but right now this function is used when deserializing and before execution where an ALL version is required
    bool areChecksValid_f(textCompilation_c* errors_par = nullptr) const;

    checksDataHub_c clone_f() const;

    action_c* actionParent_f() const;
private Q_SLOTS:
    void verifyCheckResults_f();
    void executeNextSeqCheck_f();
    //void verifyExecutionFinished_f(check_c* checkPtr_par);
Q_SIGNALS:
    void checksExecutionStarted_signal();
    void stoppingChecksExecution_signal();
    void checksExecutionStopped_signal();
    void checksExecutionFinished_signal(const bool result_par_con);
public Q_SLOTS:
};

#endif // ACTONQTSO_CHECKSDATAHUB_HPP
