#ifndef ACTONQTSO_EXECUTIONOPTIONS_HPP
#define ACTONQTSO_EXECUTIONOPTIONS_HPP

#include <QJsonObject>

class executionOptions_c
{
    //loop execution after everything has finished executing --> anyfinish is true for all the actions
    bool loopExecution_pri = false;
    //finish already executing actions but don't run anything else
    bool stopExecutingOnError_pri = true;

    //when executing actions how many threads to use
    //must be >0, default is 1, max is int32 max value
    //right now an action can only use 1 thread, but
    //each additional actions executing at the same time will require
    //1 thread each. If no extra threads, because the limit has been reached,
    //are available when an action is ready to run it will wait until one is
    int_fast32_t extraThreads_pri = 1;
    //10 seconds
    int_fast32_t killTimeoutMilliseconds_pri = 10000;
public:
    //ctor to deserialize
    executionOptions_c() = default;
    //ctor to serialize
    executionOptions_c(const bool loopExecution_par_con
            , const bool stopExecutingOnError_par_con
            , const int_fast32_t extraThreads_par_con
            , const int_fast32_t killTimeoutMilliseconds_par_con
    );
   void write_f(QJsonObject &json) const;
   void read_f(const QJsonObject &json);

   bool loopExecution_f() const;
   bool stopExecutingOnError_f() const;
   int_fast32_t extraThreads_f() const;
   int_fast32_t killTimeoutMilliseconds_f() const;

   //moving the data is on the actonBase class
   void setLoopExecution_f(const bool loopExecution_par_con);
   void setStopExecutingOnError_f(const bool stopExecutingOnError_par_con);
   void setExtraThreads_f(const int_fast32_t extraThreads_par_con);
   void setKillTimeoutMilliseconds_f(const int_fast32_t killTimeoutMilliseconds_par_con);
};

#endif // ACTONQTSO_ACTIONEXECUTIONOPTIONS_HPP
