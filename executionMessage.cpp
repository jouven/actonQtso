#include "executionMessage.hpp"

#include "QDateTime"

QString executionMessage_c::reference_f() const
{
    return reference_pri;
}

textCompilation_c executionMessage_c::text_f() const
{
    return text_pri;
}

executionMessage_c::type_ec executionMessage_c::type_f() const
{
    return type_pri;
}

qint64 executionMessage_c::timestamp_f() const
{
    return timestamp_pri;
}

QObject* executionMessage_c::objThatHasExecutionMessagesPtr_f() const
{
    return objThatHasExecutionMessagesPtr_pri;
}

void executionMessage_c::unreferenceObjThatHasExecutionMessages_f()
{
    objThatHasExecutionMessagesPtr_pri = nullptr;
}

const QMap<QString, executionMessage_c::type_ec>& stringToExecutionMessageTypeMapRef_f()
{
    static const QMap<QString, executionMessage_c::type_ec> stringToExecutionMessageTypeMap_sta_con(
    {
    //Keys are lower case because this way when reading from json the strings can be lower-cased
    //and compared, allowing to ignore case
    {	"information", executionMessage_c::type_ec::information}
    , {	"warning", executionMessage_c::type_ec::warning}
    , {	"error", executionMessage_c::type_ec::error}
    , {	"externalsdout", executionMessage_c::type_ec::externalsdout}
    , {	"externalstderr", executionMessage_c::type_ec::externalstderr}
    });
    return stringToExecutionMessageTypeMap_sta_con;
}

const std::unordered_map<executionMessage_c::type_ec, QString>& executionMessageTypeToStringUMap_ref()
{
    static const std::unordered_map<executionMessage_c::type_ec, QString> executionMessageTypeToStringUMap_sta_con(
    {
        {	executionMessage_c::type_ec::empty, "empty" }
        , {	executionMessage_c::type_ec::information, "information" }
        , {	executionMessage_c::type_ec::warning, "warning" }
        , {	executionMessage_c::type_ec::error, "error" }
        , {	executionMessage_c::type_ec::externalsdout, "externalsdout" }
        , {	executionMessage_c::type_ec::externalstderr, "externalstderr" }
    });
    return executionMessageTypeToStringUMap_sta_con;
}

QString executionMessage_c::executionMessageTypeToString_f(const executionMessage_c::type_ec executionMessageType_par_con)
{
    return executionMessageTypeToStringUMap_ref().at(executionMessageType_par_con);
}

executionMessage_c::type_ec executionMessage_c::stringToExecutionMessageType_f(const QString& string_par_con)
{
    return stringToExecutionMessageTypeMapRef_f().value(string_par_con, executionMessage_c::type_ec::empty);
}

QList<QString> executionMessage_c::executionMessageTypeStringValues_f()
{
    return stringToExecutionMessageTypeMapRef_f().keys();
}

executionMessage_c::executionMessage_c(
        const QString& reference_par_con
        , const textCompilation_c& text_par_con
        , const executionMessage_c::type_ec type_par_con
        , const uint_fast64_t timestamp_par_con)
    : id_pri(++atomicIndex_pri)
    , reference_pri(reference_par_con)
    , text_pri(text_par_con)
    , type_pri(type_par_con)
    , timestamp_pri(timestamp_par_con)
{
    if (timestamp_pri == 0)
    {
        timestamp_pri = QDateTime::currentMSecsSinceEpoch();
    }
}

executionMessage_c::executionMessage_c(
        QObject* objThatHasExecutionMessagesPtr_par
        , const QString& reference_par_con
        , const textCompilation_c& text_par_con
        , const executionMessage_c::type_ec type_par_con
        , const uint_fast64_t timestamp_par_con)
  : executionMessage_c(reference_par_con, text_par_con, type_par_con, timestamp_par_con)
{
    //NEVER CALL A DELEGATE CTOR INSIDE THE CTOR BODY
    //executionMessage_c(reference_par_con, text_par_con, type_par_con, timestamp_par_con); <-- this is wrong
    objThatHasExecutionMessagesPtr_pri = objThatHasExecutionMessagesPtr_par;
}

uint_fast64_t executionMessage_c::id_f() const
{
    return id_pri;
}
