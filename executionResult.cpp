#include "executionResult.hpp"

#include "actonDataHub.hpp"
#include "checkData.hpp"

#include <QDateTime>

#define MACRO_ADDACTIONLOG(...) \
if (parent()->parent() not_eq nullptr) \
{ \
    MACRO_ADDACTONDATAHUBLOG(static_cast<action_c*>(parent())->actonDataHubParent_f(),__VA_ARGS__); \
}

#define MACRO_ADDCHECKLOG(...) \
if (parent()->parent() not_eq nullptr and static_cast<check_c*>(parent())->parentAction_f()->actonDataHubParent_f() not_eq nullptr) \
{ \
    MACRO_ADDACTONDATAHUBLOG(static_cast<check_c*>(parent())->parentAction_f()->actonDataHubParent_f(),__VA_ARGS__); \
}

bool executionResult_c::started_f() const
{
    return started_pro;
}

bool executionResult_c::finished_f() const
{
    return finished_pro;
}


qint64 executionResult_c::startTime_f() const
{
    return startTime_pro;
}

qint64 executionResult_c::finishedTime_f() const
{
    return finishedTime_pro;
}

//bool executionResult_c::tryClear_f()
//{
//    bool resultTmp(false);
//    if (finished_pro)
//    {
//        //TODO unparent?
//        messages_pro.clear();

//        started_pro = false;
//        finished_pro = false;
//        stoppedByUser_pro = false;

//        startTime_pro = 0;
//        finishedTime_pro = 0;

//        derivedTryClear_f();
//        resultTmp = true;
//    }
//    return resultTmp;
//}

void executionResult_c::appendMessage_f(executionMessage_c* message_par_con)
{
    if (not finished_pro)
    {
        messages_pro.emplace_back(message_par_con);
        derivedAppendMessage_f(message_par_con);
        Q_EMIT messageAdded_signal(this, message_par_con);
        while (not finished_pro)
        {
            if (message_par_con->type_f() == executionMessage_c::type_ec::information)
            {
                Q_EMIT informationMessageAdded_signal(this, message_par_con);
                break;
            }
            if (message_par_con->type_f() == executionMessage_c::type_ec::error)
            {
                Q_EMIT errorMessageAdded_signal(this, message_par_con);
                break;
            }
            if (message_par_con->type_f() == executionMessage_c::type_ec::warning)
            {
                Q_EMIT warningMessageAdded_signal(this, message_par_con);
                break;
            }
            break;
        }
        Q_EMIT anyChange_signal(this);
    }
    if (finished_pro)
    {
        if (type_f() == type_ec::action)
        {
            MACRO_ADDACTIONLOG(
                        {"Message appended while execution finished, message id {0} contents {1}"
                         , message_par_con->id_f()
                         , message_par_con->text_f().toRawReplace_f()}
                        , messageType_ec::warning);
        }

        if (type_f() == type_ec::check)
        {
            MACRO_ADDCHECKLOG(
                        {"Message appended while execution finished, message id {0} contents {1}"
                         , message_par_con->id_f()
                         , message_par_con->text_f().toRawReplace_f()}
                        , messageType_ec::warning);
        }
    }
}

void executionResult_c::trySetFinished_f()
{
    if (not finished_pro)
    {
        if (derivedTrySetFinished_f())
        {
            finished_pro = true;
            if (startTime_pro not_eq 0)
            {
                finishedTime_pro = QDateTime::currentMSecsSinceEpoch();
            }
            Q_EMIT finished_signal(this);
            Q_EMIT anyChange_signal(this);
            this->disconnect();
        }
    }
}

void executionResult_c::unreferenceMessages_f(const std::vector<executionMessage_c*>& messagePtrs_par_con)
{
    for (executionMessage_c* message_ite : messagePtrs_par_con)
    {
        auto findResultTmp(std::find(messages_pro.begin(), messages_pro.end(), message_ite));
        if (findResultTmp not_eq messages_pro.end())
        {
            messages_pro.erase(findResultTmp);
        }
    }
}

const QMap<QString, executionResult_c::type_ec>& stringToExecutionResultTypeMapRef_f()
{
    static const QMap<QString, executionResult_c::type_ec> stringToExecutionResultTypeMap_sta_con(
    {
    //Keys are lower case because this way when reading from json the strings can be lower-cased
    //and compared, allowing to ignore case
    {	"action", executionResult_c::type_ec::action}
    , {	"check", executionResult_c::type_ec::check}
    });
    return stringToExecutionResultTypeMap_sta_con;
}

const std::unordered_map<executionResult_c::type_ec, QString>& executionResultTypeToStringUMap_ref()
{
    static const std::unordered_map<executionResult_c::type_ec, QString> executionResultTypeToStringUMap_sta_con(
    {
        {	executionResult_c::type_ec::empty, "empty" }
        , {	executionResult_c::type_ec::action, "Action" }
        , {	executionResult_c::type_ec::check, "Check" }
    });
    return executionResultTypeToStringUMap_sta_con;
}

QString executionResult_c::executionResultTypeToString_f(const executionResult_c::type_ec executionResultType_par_con)
{
    return executionResultTypeToStringUMap_ref().at(executionResultType_par_con);
}

executionResult_c::type_ec executionResult_c::stringToExecutionResultType_f(const QString& string_par_con)
{
    return stringToExecutionResultTypeMapRef_f().value(string_par_con, executionResult_c::type_ec::empty);
}

QList<QString> executionResult_c::executionResultTypeStringValues_f()
{
    return stringToExecutionResultTypeMapRef_f().keys();
}

executionResult_c::executionResult_c(QObject* parent_par)
    : QObject(parent_par)
    , id_pri(++atomicIndex_pri)
{}

std::vector<executionMessage_c*> executionResult_c::messages_f(const std::unordered_set<executionMessage_c::type_ec>& types_par_con) const
{
    std::vector<executionMessage_c*> messagesTmp;
    //vector requires no const
    for (executionMessage_c* message_ite : messages_pro)
    {
        if (types_par_con.empty() or types_par_con.count(message_ite->type_f()) == 1)
        {
            messagesTmp.emplace_back(message_ite);
        }
    }
    return messagesTmp;
}

textCompilation_c executionResult_c::messagesTextCompilation_f(const std::unordered_set<executionMessage_c::type_ec>& types_par_con) const
{
    textCompilation_c resultTmp;
    for (const executionMessage_c* message_ite_con : messages_pro)
    {
        if (types_par_con.empty() or types_par_con.count(message_ite_con->type_f()) == 1)
        {
            resultTmp.append_f(message_ite_con->text_f());
        }
    }
    return resultTmp;
}

QString executionResult_c::messagesStr_f(const std::unordered_set<executionMessage_c::type_ec>& types_par_con) const
{
    QString resultTmp;
    for (const executionMessage_c* message_ite_con : messages_pro)
    {
        if (types_par_con.empty() or types_par_con.count(message_ite_con->type_f()) == 1)
        {
            resultTmp.append(message_ite_con->text_f().toRawReplace_f());
        }
    }
    return resultTmp;
}

size_t executionResult_c::messageCount_f() const
{
    return messages_pro.size();
}

executionMessage_c* executionResult_c::lastMessage_f() const
{
    executionMessage_c* resultTmp(nullptr);
    if (not messages_pro.empty())
    {
        resultTmp = messages_pro.back();
    }
    return  resultTmp;
}

bool executionResult_c::hasErrorMessages_f() const
{
    bool resultTmp(false);
    for (const executionMessage_c* message_ite_con : messages_pro)
    {
        if (message_ite_con->type_f() == executionMessage_c::type_ec::error)
        {
            resultTmp = true;
            break;
        }
    }
    return resultTmp;
}

bool executionResult_c::stoppedByUser_f() const
{
    return stoppedByUser_pro;
}


void executionResult_c::setStarted_f()
{
    if (not started_pro)
    {
        started_pro = true;
        startTime_pro = QDateTime::currentMSecsSinceEpoch();
        //derivedSetStarted_f();
        Q_EMIT started_signal(this);
        Q_EMIT anyChange_signal(this);
    }
}

uint_fast64_t executionResult_c::id_f() const
{
    return id_pri;
}
