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
#include <QJsonObject>

#include <deque>

//TODO implement a field widget that has field name, source (combo), and field value
//i.e: directory to create: (field name), from action result/manual input (source), action unique id/"/home/jouven/test" (value)

//FUTURE make description optional, for each action create a description function that can be used some generic description
//that use values of that specific action as a "description"

class actionDataExecutionResult_c;
class baseActionExecution_c;

//an action or actionData is:
//what goes in a row in the mainWindow in actonQtg
//But more seriously it's an operation, creating a folder, deleting a file... that has a result/s
//the result of an action doesn't matter to the action or other actions "directly"
//(an action can use other action results but without knowing if there is any, it can't know),
//a "check" can query if an action, has ended (or not), if it's successful (or not), if there are results (or not)
//an action can have multiple checks
class EXPIMP_ACTONQTSO actionData_c //: public eines::baseClassQt_c
{
    //the id is a, fast, means to map the row position with the actionData
    int_fast64_t id_pri = 0;

    actionType_ec type_pri = actionType_ec::empty;
    //can't be empty and should/must be unique (actionDataHub_c checks this)
    QString stringId_pri;

    QString description_pri;

    //the following 2 bools, only apply if there are >0 checks

    //all the check results should be *logic and* as a final result
    //otherwise it's *or logic* (one successful check is enough to run the action)
    bool checkResultLogicAnd_pri = true;
    //when the above is false and one result is successful but not all the checks
    //have run finish already executing but don't run any check left (it's not required anyway)
    bool runAllChecksAnyway_pri = false;

    bool checksEnabled_pri = true;

    //will ignore execution operations (execute, stop, execution objects, from this obj, will return Q_NULLPTR)
    bool enabled_pri = true;

    //bellow is not "jsoned"
    QJsonObject actionDataJSON_pri;

    checksDataHub_c checkDataHub_pri;

    //it's a baseActionExecution_c (base class) because every execution object
    //has a different class but all have baseActionExecution_c as base and using the action type
    //it's possible to cast to the different execution classes or use virtual functions
    baseActionExecution_c* actionDataExecution_ptr_pri = nullptr;
    actionDataExecutionResult_c* actionDataExecutionResult_ptr_pri = nullptr;

    void clearExecutionResults_f() const;

    void deleteActionDataExecutionObject_f();
    void deleteActionDataExecutionResultObject_f();

    void deleteUsedPtrs_f();
    void execute_f();
    void examineCheckResults_f(std::vector<checkData_c*> checksRun_par);
    void prepareToRunAction_f();

    bool isKillingExecutionAfterTimeout_pri = false;

    void stopExecutingChecks_f();
    //FUTURE will tryStop first, if false, it will will try to leave the action half done, if that's possible within the action, else will kill the thread
    //each action must be modified to allow a half done state
    void kill_f();

    bool isEditable_f() const;
    //use only on sets
    bool tryClearResultsOnEdit_f();

public:
    explicit actionData_c();
    explicit actionData_c(
            const actionType_ec type_par_con
            , const QString& stringId_par_con
            , const QString& description_par_con
            , const bool checkResultLogicAnd_par_con = true
            , const bool runAllChecksAnyway_par_con = false
            , const bool checksEnabled_par_con = true
            , const bool enabled_par_con = true
            , const QJsonObject& actionDataJson_par_con = QJsonObject()
    );

    //copies everything except pointers
    actionData_c(const actionData_c& from_par_con);
    //required to copy from a pointer dereference, does the same the above
    actionData_c(actionData_c& from_par);
    //std::moves everything and nulls from_par pointers
    actionData_c(actionData_c&& from_par) noexcept;

    //copies everything except pointers and id_pri, deletes pointer objs
    actionData_c& operator=(const actionData_c& from_par_con);
    //same as above
    actionData_c& operator=(actionData_c& from_par);
    //same as move ctor
    actionData_c& operator=(actionData_c&& from_par) noexcept;

    //delete the pointer variables if they aren't nullptr/Q_NULLPTR
    ~actionData_c();

    void write_f(QJsonObject &json_ref_par) const;
    void read_f(const QJsonObject &json_par_con);

    //FUTURE dates (creation, modification with the option of hide/show in the grid)
    int_fast64_t id_f() const;
    QString stringId_f() const;
    actionType_ec type_f() const;
    QString description_f() const;
    QJsonObject actionDataJSON_f() const;
    //QJsonObject& actionDataJSON_ref_f();
    bool checksEnabled_f() const;
    bool enabled_f() const;

    checksDataHub_c* checkDataHub_ptr_f();
    const checksDataHub_c& checkDataHub_f() const;

    bool isStoppingExecution_f() const;
    bool isExecuting_f() const;
    bool killingExecutionAfterTimeout_f() const;

    void setType_f(const actionType_ec type_par_con);
    //updateFieldsThatUsedOldValue_par_con == true, returns the amount of fields updated
    int_fast32_t setStringId_f(const QString& stringId_par_con, const bool updateFieldsThatUsedOldValue_par_con = false);
    void setDescription_f(const QString& description_par_con);
    void setActionDataJSON_f(const QJsonObject& actionDataJSON_par_con);
    void setChecksEnabled_f(const bool checksEnabled_par_con);
    void setEnabled_f(const bool enabled_par_con);

    //WARNING actionDataHub_c (datahub_f) has an execute function too, executeActionDataRows_f
    //Use that call if the Action is in the datahub, only call this if the action is in the vacuum
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
};

#endif // ACTONQTSO_ACTIONDATA_HPP
