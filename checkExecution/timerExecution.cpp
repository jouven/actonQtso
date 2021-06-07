#include "timerExecution.hpp"

#include "checks/timer.hpp"
#include "../dateTimeStringFormat.hpp"

#include "textQtso/text.hpp"

#include <QTimer>
#include <QDateTime>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

timerCheckExecution_c::timerCheckExecution_c(
        checkExecutionResult_c* checkExecutionResultObj_par_con
        , timerCheck_c* timerCheckPtr_par)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
    , timerCheckPtr_pri(timerCheckPtr_par)
{}

void timerCheckExecution_c::timerFinished_f()
{
    Q_EMIT anyFinish_signal(true);
}

void timerCheckExecution_c::derivedExecute_f()
{
    //Q_EMIT executionStateChange_signal(checkExecutionState_ec::executing);
    qint64 timeoutValueTmp(0);
    while (true)
    {
        if (timerCheckPtr_pri->alarmType_f() == timerCheck_c::type_ec::timer)
        {
            timeoutValueTmp = timerCheckPtr_pri->value_f();
            break;
        }
        if (timerCheckPtr_pri->alarmType_f() == timerCheck_c::type_ec::dateTime)
        {
            qint64 currentMSecsSinceEpochTmp(QDateTime::currentMSecsSinceEpoch());
            timeoutValueTmp = timerCheckPtr_pri->value_f() - currentMSecsSinceEpochTmp;
            if (timeoutValueTmp < 0)
            {
                if (timerCheckPtr_pri->errorOnPastDateTimeValue_f())
                {
                    QDateTime dateTimeTmp(QDateTime::fromMSecsSinceEpoch(timerCheckPtr_pri->value_f()));
                    emitExecutionMessage_f({"Timer dateTime value is past: {0}", dateTimeTmp.toString(dateTimeFormat_con)}, executionMessage_c::type_ec::error);
                }
                else
                {
                    timeoutValueTmp = 0;
                }
            }
            else
            {
                //timeout value is 0 or positive, no problem
            }
            break;
        }

        break;
    }
#ifdef DEBUGJOUVEN
    //qDebug() << "timeoutValueTmp " << timeoutValueTmp << Qt::endl;
#endif
    if (timeoutValueTmp >= 0)
    {
        timer_pri = new QTimer(this);
        timer_pri->setSingleShot(true);
        QObject::connect(timer_pri, &QTimer::timeout, this, &timerCheckExecution_c::timerFinished_f);
        timer_pri->start(timeoutValueTmp);
    }
    else
    {
        Q_EMIT anyFinish_signal(false);
    }
}

void timerCheckExecution_c::derivedStop_f()
{
    timer_pri->stop();
    Q_EMIT anyFinish_signal(false);
}


