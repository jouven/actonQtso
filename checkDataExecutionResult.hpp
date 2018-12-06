#ifndef ACTONQTSO_CHECKDATARESULT_HPP
#define ACTONQTSO_CHECKDATARESULT_HPP

#include "checkMappings/checkExecutionStates.hpp"

#include <QObject>
#include <QString>

class checkData_c;
//this class it to deal in a generic way with all the check execution result/s
//a bit different from the action one, since the only things that matter are the result (true or false) and if there is any error
//state kept just to hint to where it fails when it fails
class checkDataExecutionResult_c : public QObject
{
    Q_OBJECT

    //description of any errors during the execution of this check
    QString errors_pri;

    checkData_c* const parent_ptr_pri;

    checkExecutionState_ec executionState_pri = checkExecutionState_ec::initial;

    bool result_pri = false;

    bool started_pri = false;
    qint64 startTime_pri = 0;
    bool finished_pri = false;
    qint64 finishedTime_pri = 0;

    bool stoppedByUser_pri = false;
public:
    explicit checkDataExecutionResult_c(
            checkData_c* const parentActionData_par_ptr_con
    );

    QString error_f() const;

    checkExecutionState_ec state_f() const;
    checkData_c* parent_ptr_f() const;

    bool started_f() const;
    bool finished_f() const;
    bool result_f() const;
    bool stoppedByUser_f() const;

    qint64 startTime_f() const;
    qint64 finishedTime_f() const;

    void clear_f();

Q_SIGNALS:
    void errorUpdated_signal(checkData_c* checkData_par_ptr);

    void executionStateUpdated_signal(checkData_c* checkData_par_ptr);

    void started_signal(checkData_c* checkData_par_ptr);
    void finished_signal(checkData_c* checkData_par_ptr);

    void stopping_signal(checkData_c* checkData_par_ptr);
    void stopped_signal(checkData_c* checkData_par_ptr);

    void resultsCleared_signal(checkData_c* checkData_par_ptr);
public Q_SLOTS:
    void appendError_f(const QString& error_par_con);

    //some of the checkExecutionState are final, like finished, after the object can't be modified anymore
    //the other fields must be set first before transitioning into final state
    void setExecutionState_f(const checkExecutionState_ec checkExecutionState_par_con);

    void setStarted_f();
    void setFinished_f(const bool checkResult_par_con);
};

#endif // ACTONQTSO_CHECKDATARESULT_HPP
