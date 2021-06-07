#include "actonExecutionOptions.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"
#include "threadedFunctionQtso/threadedFunctionQt.hpp"

#ifdef DEBUGJOUVEN
#include "essentialQtso/essentialQt.hpp"
#endif

#include <QJsonObject>

const QMap<QString, executionOptions_c::executionLoopType_ec> executionOptions_c::strToExecutionLoopTypeMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"loopxtimes", executionOptions_c::executionLoopType_ec::loopXTimes}
    , {	"forever", executionOptions_c::executionLoopType_ec::forever}
});

const std::unordered_map<executionOptions_c::executionLoopType_ec, QString> executionOptions_c::executionLoopTypeToStrUMap_sta_con(
{
    {	executionOptions_c::executionLoopType_ec::loopXTimes, "loopXTimes" }
    , {	executionOptions_c::executionLoopType_ec::forever, "forever" }
});

executionOptions_c::executionLoopType_ec executionOptions_c::executionLoopType_f() const
{
    return executionLoopType_pri;
}

//bool executionOptions_c::stopExecutingOnError_f() const
//{
//    return stopExecutingOnError_pri;
//}

void executionOptions_c::setExecutionLoopType_f(const executionOptions_c::executionLoopType_ec executionLoopType_par_con)
{
    executionLoopType_pri = executionLoopType_par_con;
}

uint_fast64_t executionOptions_c::loopXTimesCount_f() const
{
    return loopXTimesCount_pri;
}

//bool executionOptions_c::stopExecutingOnError_f() const
//{
//    return stopExecutingOnError_pri;
//}

void executionOptions_c::setLoopXTimesCount_f(const uint_fast64_t loopXTimesCount_par_con)
{
    loopXTimesCount_pri = loopXTimesCount_par_con;
}

//void executionOptions_c::setStopExecutingOnError_f(const bool stopExecutingOnError_par_con)
//{
//    stopExecutingOnError_pri = stopExecutingOnError_par_con;
//}

uint_fast64_t executionOptions_c::extraThreads_f() const
{
    return extraThreads_pri;
}

void executionOptions_c::setExtraThreads_f(const uint_fast64_t extraThreads_par_con)
{
    extraThreads_pri = extraThreads_par_con;
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
        const executionLoopType_ec executionLoopType_par_con
        , const uint_fast64_t loopXTimesCount_par_con
        //, const bool stopExecutingOnError_par_con
        , const uint_fast64_t extraThreads_par_con
)
    : executionLoopType_pri(executionLoopType_par_con)
    , loopXTimesCount_pri(loopXTimesCount_par_con)
    //, stopExecutingOnError_pri(stopExecutingOnError_par_con)
    , extraThreads_pri(extraThreads_par_con)
{}

void executionOptions_c::write_f(QJsonObject& json_par) const
{
    json_par["executionLoopType"] = executionLoopTypeToStrUMap_sta_con.at(executionLoopType_pri);
    json_par["loopXTimesCount"] = QString::number(loopXTimesCount_pri);
    //json["stopExecutionOnError"] = stopExecutingOnError_pri;
    //json number limits... save string
    json_par["extraThreads"] = QString::number(extraThreads_pri);
    if (stringParserMap_pri not_eq nullptr)
    {
        QJsonObject stringParserJSONObjectTmp;
        stringParserMap_pri->write_f(stringParserJSONObjectTmp);
        json_par["stringParserMap"] = stringParserJSONObjectTmp;
    }
}

void executionOptions_c::read_f(const QJsonObject& json_par_con)
{
    if (json_par_con["executionLoopType"].isString())
    {
        executionLoopType_pri = strToExecutionLoopTypeMap_sta_con.value(json_par_con["executionLoopType"].toString().toLower());
    }
    //stopExecutingOnError_pri = json["stopExecutionOnError"].toBool(true);
    //json number limits... load from string
    if (json_par_con["loopXTimesCount"].isString())
    {
        loopXTimesCount_pri = json_par_con["loopXTimesCount"].toString(QString::number(loopXTimesCount_pri)).toLongLong();
    }
    if (json_par_con["extraThreads"].isString())
    {
#ifdef DEBUGJOUVEN
        //QOUT_TS("QString::number(extraThreads_pri)).toLongLong() " << json_par_con["extraThreads"].toString(QString::number(extraThreads_pri)).toLongLong() << Qt::endl);
#endif
        extraThreads_pri = json_par_con["extraThreads"].toString(QString::number(extraThreads_pri)).toLongLong();
    }
    if (json_par_con["stringParserMap"].isUndefined())
    {
        //use default value (if the pointer has an object)
    }
    else
    {
        if (stringParserMap_pri not_eq nullptr)
        {
            stringParserMap_pri->read_f(json_par_con["stringParserMap"].toObject());
        }
    }
}

bool executionOptions_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool isValidResultTmp(false);
    while (true)
    {
//        if (loopXTimesCount_pri < 0)
//        {
//            APPENDTEXTPTR(errorsPtr_par, "Loop X times count must be a positive number");
//            break;
//        }
        if (extraThreads_pri < 1)
        {
            APPENDTEXTPTR(errorsPtr_par, "Extra threads must be >1");
            break;
        }
//        if (killTimeoutMilliseconds_pri < 0)
//        {
//            APPENDTEXTPTR(errorsPtr_par, "Kill timeout must be a positive number");
//            break;
//        }

        isValidResultTmp = true;
        break;
    }
    return isValidResultTmp;
}
