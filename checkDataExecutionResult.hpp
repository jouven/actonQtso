#ifndef ACTONQTSO_CHECKDATARESULT_HPP
#define ACTONQTSO_CHECKDATARESULT_HPP

#include "checkMappings/checkExecutionStates.hpp"

#include <QObject>
#include <QString>

#include <vector>

#include "crossPlatformMacros.hpp"

class checkData_c;
//this class it to deal in a generic way with all the check execution result/s
//a bit different from the action one, since the only things that matter are the result (true or false) and if there is any error
//state kept just to hint to where it fails when it fails
class EXPIMP_ACTONQTSO checkDataExecutionResult_c : public QObject
{
    Q_OBJECT

    //description of any errors during the execution of this check
    QString errors_pri;

    checkData_c* const parent_ptr_pri;

    std::vector<checkExecutionState_ec> executionStateVector_pri = { checkExecutionState_ec::initial };

    bool result_pri = false;

    bool started_pri = false;
    qint64 startTime_pri = 0;
    bool finished_pri = false;
    qint64 finishedTime_pri = 0;

    bool stoppedByUser_pri = false;

    void setStarted_f();
public:
    explicit checkDataExecutionResult_c(
            checkData_c* const parentActionData_par_ptr_con
    );

    QString error_f() const;

    std::vector<checkExecutionState_ec> executionStateVector_f() const;
    checkExecutionState_ec lastState_f() const;
    checkData_c* parent_ptr_f() const;

    bool started_f() const;
    bool finished_f() const;
    bool result_f() const;
    bool stoppedByUser_f() const;

    qint64 startTime_f() const;
    qint64 finishedTime_f() const;

    //will clear if finished
    bool tryClear_f();

Q_SIGNALS:
    void executionStateUpdated_signal(checkData_c* checkData_par_ptr);

    void started_signal(checkData_c* checkData_par_ptr);
    void finished_signal(checkData_c* checkData_par_ptr);

    void stopping_signal(checkData_c* checkData_par_ptr);
    void stopped_signal(checkData_c* checkData_par_ptr);

    void preparing_signal(checkData_c* actionData_ptr_par_con);
    void executing_signal(checkData_c* actionData_ptr_par_con);
    void error_signal(checkData_c* actionData_ptr_par_con);

    void resultsCleared_signal(checkData_c* checkData_par_ptr);
    //the slots should only be used by the check/execution object
public Q_SLOTS:
    //changes the execution state to error
    void appendError_f(const QString& error_par_con);

    //some of the checkExecutionState are final, like error, after the object can't be modified anymore
    bool trySetExecutionState_f(const checkExecutionState_ec checkExecutionState_par_con);

    void trySetFinished_f();
};

#endif // ACTONQTSO_CHECKDATARESULT_HPP
