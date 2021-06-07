#ifndef ACTONQTSO_EXECUTIONOPTIONS_HPP
#define ACTONQTSO_EXECUTIONOPTIONS_HPP

#include "crossPlatformMacros.hpp"

#include <QMap>
#include <unordered_map>

class QJsonObject;

class stringParserMap_c;

class textCompilation_c;

class EXPIMP_ACTONQTSO executionOptions_c
{
public:
    enum class executionLoopType_ec
    {
        empty = 0
        //keep launching the actions X times
        , loopXTimes = 1
        //keep launching the actions forever
        , forever = 2
    };

    static EXPIMP_ACTONQTSO const QMap<QString, executionLoopType_ec> strToExecutionLoopTypeMap_sta_con;
    static EXPIMP_ACTONQTSO const std::unordered_map<executionLoopType_ec, QString> executionLoopTypeToStrUMap_sta_con;
private:
    //single actions or overall execution don't loop by default
    //actions have an option to keep executing after they finish "forever"
    //this option is for overall execution, if all the actions finish normally the overall execution will start again forever/loopXTimesCount_pri times
    executionLoopType_ec executionLoopType_pri = executionLoopType_ec::loopXTimes;
    //how many execution loops/repeats, only applies when executionLoopType_pri = executionLoopTypeType_ec::loopXTimes
    //1 = repeat once (after the first run), 2 = repeat 2 times
    uint_fast64_t loopXTimesCount_pri = 0;

//    //when an error happens stop executing actions and don't run anything else
//    bool stopExecutingOnError_pri = true;

    //when executing actions how many extra threads to use (there is always the main-GUI thread)
    //must be >0, default is 1, max is int64 max value
    //right now an action/check will use at most 1 extra thread,
    //simple actions/checks will run on the main thread, more "intensive" ones will run on their own thread.
    //If no extra threads, because the limit has been reached,
    //are available when an action is ready to run it will wait until one is
    //Even with 1 thread the overall execution can run and finish no problem
    //20210222 +1 because signalProxyQtso requires one
    uint_fast64_t extraThreads_pri = 2;

    stringParserMap_c* stringParserMap_pri = nullptr;
public:
    //ctor to deserialize
    executionOptions_c() = default;
    //ctor to serialize
    executionOptions_c(
            const executionLoopType_ec executionLoopType_par_con
            , const uint_fast64_t loopXTimesCount_par_con
            //, const bool stopExecutingOnError_par_con
            , const uint_fast64_t extraThreads_par_con
   );
   void write_f(QJsonObject &json_par) const;
   //stringParserMap must be set for the json parser to read anything into it
   //otherwise any stringTrigger/s config is ignored
   //will replace existing values except the stringParser,
   //parsers will be added to the existing ones BUT conflicting, same stringTrigger key, ones will replace the old
   void read_f(const QJsonObject &json_par_con);

   executionLoopType_ec executionLoopType_f() const;
   uint_fast64_t loopXTimesCount_f() const;
   //bool stopExecutingOnError_f() const;
   uint_fast64_t extraThreads_f() const;

   void setExecutionLoopType_f(const executionLoopType_ec executionLoopType_par_con);
   void setLoopXTimesCount_f(const uint_fast64_t loopXTimesCount_par_con);
   //void setStopExecutingOnError_f(const bool stopExecutingOnError_par_con);
   void setExtraThreads_f(const uint_fast64_t extraThreads_par_con);

   stringParserMap_c* stringParserMap_f() const;
   void setStringParserMap_f(stringParserMap_c* stringParserMap_par);

   bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};

#endif // ACTONQTSO_ACTIONEXECUTIONOPTIONS_HPP
