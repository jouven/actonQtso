#include "runProcess.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "../actonDataHub.hpp"

#include <QProcess>
#include <QJsonArray>

QString runProcessAction_c::workingDirectory_f() const
{
    return workingDirectory_pri;
}

QString runProcessAction_c::workingDirectoryParsed_f() const
{
    COPYPARSERETURNVAR(workingDirectory_pri);
}

void runProcessAction_c::setWorkingDirectory_f(const QString& workingDirectory_par_con)
{
    workingDirectory_pri = workingDirectory_par_con;
}

QHash<QString, environmentPairConfig_c> runProcessAction_c::environmentToAdd_f() const
{
    return environmentToAdd_pri;
}

QHash<QString, environmentPairConfig_c> runProcessAction_c::environmentToAddParsed_f() const
{
    QHash<QString, environmentPairConfig_c> environmentToAddParsedTmp;
    environmentToAddParsedTmp.reserve(environmentToAdd_pri.size());
    QHash<QString, environmentPairConfig_c>::const_iterator iteratorTmp(environmentToAdd_pri.constBegin());
    while (iteratorTmp not_eq environmentToAdd_pri.constEnd())
    {
        QString environmentKeyParsedTmp(iteratorTmp.key());
        actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->executeForString(std::addressof(environmentKeyParsedTmp));
        environmentToAddParsedTmp.insert(environmentKeyParsedTmp, iteratorTmp.value());
        ++iteratorTmp;
    }
    return environmentToAddParsedTmp;
}

void runProcessAction_c::setEnvironmentToAdd_f(const QHash<QString, environmentPairConfig_c>& environmentToAdd_par_con)
{
    environmentToAdd_pri = environmentToAdd_par_con;
}

bool runProcessAction_c::useActonEnvironment_f() const
{
    return useActonEnvironment_pri;
}

void runProcessAction_c::setUseActonEnvironment_f(const bool useActonEnvironment_par_con)
{
    useActonEnvironment_pri = useActonEnvironment_par_con;
}

runProcessAction_c::runProcessAction_c(
        const QString& processPath_par_con
        , const std::vector<argument_c>& arguments_par_con
        , const QString& workingDirectory_par_con
        , const bool useProcessEnvironment_par_con
        , const QHash<QString, environmentPairConfig_c>& environmentToAdd_par_con)
    : processPath_pri(processPath_par_con)
    , arguments_pri(arguments_par_con)
    , workingDirectory_pri(workingDirectory_par_con)
    , useActonEnvironment_pri(useProcessEnvironment_par_con)
    , environmentToAdd_pri(environmentToAdd_par_con)
{}

void runProcessAction_c::write_f(QJsonObject& json_par) const
{
    json_par["processPath"] = processPath_pri;
    if (not arguments_pri.empty())
    {
        QJsonArray argumentArrayTmp;
        for (const argument_c& argument_ite_con : arguments_pri)
        {
            QJsonObject tmp;
            argument_ite_con.write_f(tmp);
            argumentArrayTmp.append(tmp);
        }
        json_par["arguments"] = argumentArrayTmp;
    }
    if (not environmentToAdd_pri.isEmpty())
    {
        QJsonArray environmentPairArray;
        QHash<QString, environmentPairConfig_c>::const_iterator iteratorTmp = environmentToAdd_pri.constBegin();
        while (iteratorTmp != environmentToAdd_pri.constEnd())
        {
            QJsonObject pairTmp;
            pairTmp["key"] = iteratorTmp.key();
            pairTmp["value"] = iteratorTmp.value().environmentValue();
            pairTmp["enabled"] = iteratorTmp.value().enabled_f();
            environmentPairArray.append(pairTmp);
            ++iteratorTmp;
        }
        json_par["environmentToAdd"] = environmentPairArray;
    }
    if (not workingDirectory_pri.isEmpty())
    {
        json_par["workingDirectory"] = workingDirectory_pri;
    }
    json_par["useActonEnvironment"] = useActonEnvironment_pri;
}

void runProcessAction_c::read_f(const QJsonObject& json_par_con)
{
    processPath_pri = json_par_con["processPath"].toString();
    QJsonArray jsonArrayArgumentsTmp(json_par_con["arguments"].toArray());
    if (not jsonArrayArgumentsTmp.isEmpty())
    {
        arguments_pri.reserve(jsonArrayArgumentsTmp.size());
        for (const QJsonValueRef& jsonArrayItem_ite_con : jsonArrayArgumentsTmp)
        {
            argument_c tmp;
            tmp.read_f(jsonArrayItem_ite_con.toObject());
            arguments_pri.emplace_back(tmp);
        }
    }
    if (not json_par_con["environmentToAdd"].isUndefined())
    {
        QJsonArray pairsTmp(json_par_con["environmentToAdd"].toArray());
        if (not pairsTmp.isEmpty())
        {
            environmentToAdd_pri.reserve(pairsTmp.size());
            for (const QJsonValueRef& jsonArrayItem_ite_con : pairsTmp)
            {
                QJsonObject jsonObjectTmp(jsonArrayItem_ite_con.toObject());
                environmentPairConfig_c environmentPairTmp(jsonObjectTmp["value"].toString(), jsonObjectTmp["enabled"].toBool());
                environmentToAdd_pri.insert(jsonObjectTmp["key"].toString(), environmentPairTmp);
            }
        }
    }
    workingDirectory_pri = json_par_con["workingDirectory"].toString();
    useActonEnvironment_pri = json_par_con["useActonEnvironment"].toBool();
}

QString runProcessAction_c::processPath_f() const
{
    return processPath_pri;
}

QString runProcessAction_c::processPathParsed_f() const
{
    COPYPARSERETURNVAR(processPath_pri);
}

void runProcessAction_c::setProcessPath_f(const QString& processPath_par_con)
{
    processPath_pri = processPath_par_con;
}

std::vector<argument_c> runProcessAction_c::arguments_f() const
{
    return arguments_pri;
}

void runProcessAction_c::setArguments_f(const std::vector<argument_c>& arguments_par_con)
{
    arguments_pri = arguments_par_con;
}

bool argument_c::enabled_f() const
{
    return enabled_pri;
}

void argument_c::setEnabled_f(const bool enabled_par_con)
{
    enabled_pri = enabled_par_con;
}

argument_c::argument_c(const QString& argument_par_con
        , const bool enabled_par_con)
    : argument_pri(argument_par_con)
    , enabled_pri(enabled_par_con)
{

}

void argument_c::write_f(QJsonObject& json_par) const
{
    json_par["value"] = argument_pri;
    json_par["enabled"] = enabled_pri;
}

void argument_c::read_f(const QJsonObject& json_par_con)
{
    argument_pri = json_par_con["value"].toString();
    enabled_pri = json_par_con["enabled"].toBool();
}

QString argument_c::argument_f() const
{
    return argument_pri;
}

QString argument_c::argumentParsed_f() const
{
    COPYPARSERETURNVAR(argument_pri);
}

void argument_c::setArgument_f(const QString& value_par_con)
{
    argument_pri = value_par_con;
}

environmentPairConfig_c::environmentPairConfig_c(
        const QString& environmentValue_par_con
        , const bool enabled_par_con)
    : environmentValue_pri(environmentValue_par_con)
    , enabled_pri(enabled_par_con)
{}

void environmentPairConfig_c::write_f(QJsonObject& json_par) const
{
    json_par["value"] = environmentValue_pri;
    json_par["enabled"] = enabled_pri;
}

void environmentPairConfig_c::read_f(const QJsonObject& json_par_con)
{
    environmentValue_pri = json_par_con["value"].toString();
    enabled_pri = json_par_con["enabled"].toBool();
}

QString environmentPairConfig_c::environmentValue() const
{
    return environmentValue_pri;
}

QString environmentPairConfig_c::environmentValueParsed() const
{
    COPYPARSERETURNVAR(environmentValue_pri);
}

void environmentPairConfig_c::setEnvironmentValue_f(const QString& environmentValue_par_con)
{
    environmentValue_pri = environmentValue_par_con;
}

bool environmentPairConfig_c::enabled_f() const
{
    return enabled_pri;
}

void environmentPairConfig_c::setEnabled_f(const bool enabled_par_con)
{
    enabled_pri = enabled_par_con;
}
