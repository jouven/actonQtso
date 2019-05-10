#include "actonExecutionOptions.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include <QJsonObject>

bool executionOptions_c::loopExecution_f() const
{
    return loopExecution_pri;
}

//bool executionOptions_c::stopExecutingOnError_f() const
//{
//    return stopExecutingOnError_pri;
//}

void executionOptions_c::setLoopExecution_f(const bool loopExecution_par_con)
{
    loopExecution_pri = loopExecution_par_con;
}

//void executionOptions_c::setStopExecutingOnError_f(const bool stopExecutingOnError_par_con)
//{
//    stopExecutingOnError_pri = stopExecutingOnError_par_con;
//}

int_fast32_t executionOptions_c::extraThreads_f() const
{
    return extraThreads_pri;
}

void executionOptions_c::setExtraThreads_f(const int_fast32_t extraThreads_par_con)
{
    extraThreads_pri = extraThreads_par_con;
}

int_fast32_t executionOptions_c::killTimeoutMilliseconds_f() const
{
    return killTimeoutMilliseconds_pri;
}

void executionOptions_c::setKillTimeoutMilliseconds_f(const int_fast32_t killTimeoutMilliseconds_par_con)
{
    killTimeoutMilliseconds_pri = killTimeoutMilliseconds_par_con;
}

stringParserMap_c* executionOptions_c::stringParserMap_f() const
{
    return stringParserMap_pri;
}

void executionOptions_c::setStringParserMap_f(stringParserMap_c* stringParserMap_par)
{
    stringParserMap_pri = stringParserMap_par;
}

executionOptions_c::executionOptions_c(
        const bool loopExecution_par_con
        //, const bool stopExecutingOnError_par_con
        , const int_fast32_t extraThreads_par_con
        , const int_fast32_t killTimeoutMilliseconds_par_con
)
    : loopExecution_pri(loopExecution_par_con)
    //, stopExecutingOnError_pri(stopExecutingOnError_par_con)
    , extraThreads_pri(extraThreads_par_con)
    , killTimeoutMilliseconds_pri(killTimeoutMilliseconds_par_con)
{}

void executionOptions_c::write_f(QJsonObject& json_par) const
{
    json_par["loopExecution"] = loopExecution_pri;
    //json["stopExecutionOnError"] = stopExecutingOnError_pri;
    //json number limits... save string
    json_par["extraThreads"] = QString::number(extraThreads_pri);
    json_par["killTimeoutMilliseconds"] = QString::number(killTimeoutMilliseconds_pri);
    if (stringParserMap_pri not_eq nullptr)
    {
        QJsonObject stringParserJSONObjectTmp;
        stringParserMap_pri->write_f(stringParserJSONObjectTmp);
        json_par["stringParserMap"] = stringParserJSONObjectTmp;
    }
}

void executionOptions_c::read_f(const QJsonObject& json_par_con)
{
    loopExecution_pri = json_par_con["loopExecution"].toBool(false);
    //stopExecutingOnError_pri = json["stopExecutionOnError"].toBool(true);
    //json number limits... load from string
    extraThreads_pri = json_par_con["extraThreads"].toString("1").toLongLong();
    killTimeoutMilliseconds_pri = json_par_con["killTimeoutMilliseconds"].toString("10000").toLongLong();
    if (json_par_con["stringParserMap"].isUndefined())
    {

    }
    else
    {
        stringParserMap_pri = new stringParserMap_c;
        stringParserMap_pri->read_f(json_par_con["stringParserMap"].toObject());
    }
}
