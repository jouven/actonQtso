#include "runProcess.hpp"

#include "../actionExecution/runProcessExecution.hpp"
#include "../actionMappings/actionStrMapping.hpp"
#include "../actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"

#include <QProcess>
#include <QJsonArray>

QString runProcessData_c::workingDirectory_f() const
{
    return workingDirectory_pro;
}

QString runProcessData_c::workingDirectoryParsed_f() const
{
    COPYPARSERETURNVAR(workingDirectory_pro);
}

void runProcessData_c::setWorkingDirectory_f(const QString& workingDirectory_par_con)
{
    workingDirectory_pro = workingDirectory_par_con;
}

QHash<QString, environmentPairConfig_c> runProcessData_c::environmentToAdd_f() const
{
    return environmentToAdd_pro;
}

QHash<QString, environmentPairConfig_c> runProcessData_c::environmentToAddParsed_f() const
{
    QHash<QString, environmentPairConfig_c> environmentToAddParsedTmp;
    environmentToAddParsedTmp.reserve(environmentToAdd_pro.size());
    QHash<QString, environmentPairConfig_c>::const_iterator iteratorTmp(environmentToAdd_pro.constBegin());
    while (iteratorTmp not_eq environmentToAdd_pro.constEnd())
    {
        QString environmentKeyParsedTmp(iteratorTmp.key());
        actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->executeForString_f(std::addressof(environmentKeyParsedTmp));
        environmentToAddParsedTmp.insert(environmentKeyParsedTmp, iteratorTmp.value());
        ++iteratorTmp;
    }
    return environmentToAddParsedTmp;
}

void runProcessData_c::setEnvironmentToAdd_f(const QHash<QString, environmentPairConfig_c>& environmentToAdd_par_con)
{
    environmentToAdd_pro = environmentToAdd_par_con;
}

bool runProcessData_c::useActonEnvironment_f() const
{
    return useActonEnvironment_pro;
}

void runProcessData_c::setUseActonEnvironment_f(const bool useActonEnvironment_par_con)
{
    useActonEnvironment_pro = useActonEnvironment_par_con;
}

bool runProcessData_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
        if (processPathParsed_f().isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Process path is empty");
            break;
        }
        else
        {
            //it's valid
        }

        {
            text_c errorTextTmp;
            if (isValidStringSize_f(processPathParsed_f(), 255, std::addressof(errorTextTmp), "Process path is too long: {0} (maximum length is {1})"))
            {
                //it's valid
            }
            else
            {
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
                break;
            }
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}

runProcessData_c::runProcessData_c(
        const QString& processPath_par_con
        , const std::vector<argument_c>& arguments_par_con
        , const QString& workingDirectory_par_con
        , const bool useProcessEnvironment_par_con
        , const QHash<QString, environmentPairConfig_c>& environmentToAdd_par_con)
    : processPath_pro(processPath_par_con)
    , arguments_pro(arguments_par_con)
    , workingDirectory_pro(workingDirectory_par_con)
    , useActonEnvironment_pro(useProcessEnvironment_par_con)
    , environmentToAdd_pro(environmentToAdd_par_con)
{}

void runProcessAction_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["processPath"] = processPath_pro;
    if (not arguments_pro.empty())
    {
        QJsonArray argumentArrayTmp;
        for (const argument_c& argument_ite_con : arguments_pro)
        {
            QJsonObject tmp;
            argument_ite_con.write_f(tmp);
            argumentArrayTmp.append(tmp);
        }
        json_par["arguments"] = argumentArrayTmp;
    }
    if (not environmentToAdd_pro.isEmpty())
    {
        QJsonArray environmentPairArray;
        QHash<QString, environmentPairConfig_c>::const_iterator iteratorTmp = environmentToAdd_pro.constBegin();
        while (iteratorTmp not_eq environmentToAdd_pro.constEnd())
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
    if (not workingDirectory_pro.isEmpty())
    {
        json_par["workingDirectory"] = workingDirectory_pro;
    }
    json_par["useActonEnvironment"] = useActonEnvironment_pro;
}

void runProcessAction_c::derivedRead_f(const QJsonObject& json_par_con)
{
    processPath_pro = json_par_con["processPath"].toString();
    QJsonArray jsonArrayArgumentsTmp(json_par_con["arguments"].toArray());
    if (not jsonArrayArgumentsTmp.isEmpty())
    {
        arguments_pro.reserve(jsonArrayArgumentsTmp.size());
        for (const QJsonValueRef& jsonArrayItem_ite_con : jsonArrayArgumentsTmp)
        {
            argument_c tmp;
            tmp.read_f(jsonArrayItem_ite_con.toObject());
            arguments_pro.emplace_back(tmp);
        }
    }
    if (not json_par_con["environmentToAdd"].isUndefined())
    {
        QJsonArray pairsTmp(json_par_con["environmentToAdd"].toArray());
        if (not pairsTmp.isEmpty())
        {
            environmentToAdd_pro.reserve(pairsTmp.size());
            for (const QJsonValueRef& jsonArrayItem_ite_con : pairsTmp)
            {
                QJsonObject jsonObjectTmp(jsonArrayItem_ite_con.toObject());
                environmentPairConfig_c environmentPairTmp;
                environmentPairTmp.read_f(jsonObjectTmp);
                environmentToAdd_pro.insert(jsonObjectTmp["key"].toString(), environmentPairTmp);
            }
        }
    }
    workingDirectory_pro = json_par_con["workingDirectory"].toString();
    if (json_par_con["useActonEnvironment"].isBool())
    {
        useActonEnvironment_pro = json_par_con["useActonEnvironment"].toBool();
    }
}

bool runProcessAction_c::derivedIsValidAction_f(textCompilation_c* errors_par) const
{
    return isFieldsDataValid_f(errors_par);
}

action_c* runProcessAction_c::derivedClone_f() const
{
    //slice and dice
    runProcessData_c runProcessDataTmp(*this);
    actionData_c actionDataTmp(*this);
    return new runProcessAction_c(actionDataTmp, runProcessDataTmp);
}

baseActionExecution_c* runProcessAction_c::createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par)
{
    return new runProcessActionExecution_c(actionDataExecutionResult_ptr_par, this);
}

actionType_ec runProcessAction_c::type_f() const
{
    return actionType_ec::runProcess;
}

QString runProcessAction_c::typeStr_f() const
{
    return actionTypeToStrUMap_ext_con.at(type_f());
}

runProcessAction_c::runProcessAction_c(const actionData_c& actionData_par_con, const runProcessData_c& runProcessData_par_con)
    : action_c(actionData_par_con)
    , runProcessData_c(runProcessData_par_con)
{
}

void runProcessAction_c::updateRunProcessData_f(const runProcessData_c& runProcessData_par_con)
{
    this->runProcessData_c::operator=(runProcessData_par_con);
}

QString runProcessData_c::processPath_f() const
{
    return processPath_pro;
}

QString runProcessData_c::processPathParsed_f() const
{
    COPYPARSERETURNVAR(processPath_pro);
}

void runProcessData_c::setProcessPath_f(const QString& processPath_par_con)
{
    processPath_pro = processPath_par_con;
}

std::vector<argument_c> runProcessData_c::arguments_f() const
{
    return arguments_pro;
}

void runProcessData_c::setArguments_f(const std::vector<argument_c>& arguments_par_con)
{
    arguments_pro = arguments_par_con;
}

bool argument_c::enabled_f() const
{
    return enabled_pri;
}

void argument_c::setEnabled_f(const bool enabled_par_con)
{
    enabled_pri = enabled_par_con;
}

argument_c::argument_c(
        const QString& argument_par_con
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
    if (json_par_con["enabled"].isBool())
    {
        enabled_pri = json_par_con["enabled"].toBool();
    }
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
    if (json_par_con["enabled"].isBool())
    {
        enabled_pri = json_par_con["enabled"].toBool();
    }
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
