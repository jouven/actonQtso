#include "deleteFileDir.hpp"

#include "../actionExecution/deleteFileDirExecution.hpp"
#include "../actionMappings/actionStrMapping.hpp"
#include "../actonDataHub.hpp"
#include "../reused/stringAlgo.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"

#include <QJsonObject>

QString deleteFileDirData_c::path_f() const
{
    return path_pro;
}

QString deleteFileDirData_c::pathParsed_f() const
{
    COPYPARSERETURNVAR(path_pro);
}

void deleteFileDirData_c::setPath_f(const QString& path_par_con)
{
    path_pro = path_par_con;
}

bool deleteFileDirData_c::errorIfNotExists_f() const
{
    return errorIfNotExists_pro;
}

void deleteFileDirData_c::setErrorIfNotExists_f(const bool errorIfNotExists_par_con)
{
    errorIfNotExists_pro = errorIfNotExists_par_con;
}

bool deleteFileDirData_c::onlyIfEmpty_f() const
{
    return onlyIfEmpty_pro;
}

void deleteFileDirData_c::setOnlyIfEmpty_f(const bool onlyIfEmpty_par_con)
{
    onlyIfEmpty_pro = onlyIfEmpty_par_con;
}

bool deleteFileDirData_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
        text_c errorTextTmp;
        if (isValidStringSize_f(pathParsed_f(), 255, std::addressof(errorTextTmp), "Path is too long: {0} (maximum length is {1})"))
        {
            //it's valid
        }
        else
        {
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
            break;
        }
        if (pathParsed_f().isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Path is empty")
            break;
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}

deleteFileDirData_c::deleteFileDirData_c(
        const QString& path_par_con
        , const bool errorIfNotExists_par_con
        , const bool onlyIfEmpty_par_con)
    : path_pro(path_par_con)
    , errorIfNotExists_pro(errorIfNotExists_par_con)
    , onlyIfEmpty_pro(onlyIfEmpty_par_con)
{}

void deleteFileDirAction_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["path"] = path_pro;
    json_par["errorIfNotExists"] = errorIfNotExists_pro;
    json_par["onlyIfEmpty"] = onlyIfEmpty_pro;
}

void deleteFileDirAction_c::derivedRead_f(const QJsonObject& json_par_con)
{
    path_pro = json_par_con["path"].toString();
    if (json_par_con["errorIfNotExists"].isBool())
    {
        errorIfNotExists_pro = json_par_con["errorIfNotExists"].toBool();
    }
    if (json_par_con["onlyIfEmpty"].isBool())
    {
        onlyIfEmpty_pro = json_par_con["onlyIfEmpty"].toBool();
    }
}

bool deleteFileDirAction_c::derivedIsValidAction_f(textCompilation_c* errors_par) const
{
    return isFieldsDataValid_f(errors_par);
}

uint_fast64_t deleteFileDirAction_c::derivedUpdateStringTriggerDependecies_f(const QString& oldStringTrigger_par_con, const QString& newStringTrigger_par_con)
{
    return replaceSubString_f(path_pro, oldStringTrigger_par_con, newStringTrigger_par_con);
}

uint_fast64_t deleteFileDirAction_c::derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const
{
    return vectorQStringCountSubString_f(stringTrigger_par_con, {path_pro});
}

QSet<QString> deleteFileDirAction_c::derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const
{
    QSet<QString> resultTmp;
    for (const QString& searchValue_ite_con : searchValues_par_con)
    {
        if (vectorQStringCountSubString_f(searchValue_ite_con, {path_pro}, true) > 0)
        {
            resultTmp.insert(searchValue_ite_con);
        }
    }
    return resultTmp;
}

action_c* deleteFileDirAction_c::derivedClone_f() const
{
    //slice and dice
    deleteFileDirData_c createDirectoryDataTmp(*this);
    actionData_c actionDataTmp(*this);
    return new deleteFileDirAction_c(actionDataTmp, createDirectoryDataTmp);
}

baseActionExecution_c* deleteFileDirAction_c::createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par)
{
    return new deleteFiledirActionExecution_c(actionDataExecutionResult_ptr_par, this);
}

actionType_ec deleteFileDirAction_c::type_f() const
{
    return actionType_ec::deleteFileDir;
}

//QString deleteFileDirAction_c::typeStr_f() const
//{
//    return actionTypeToStrUMap_ext_con.at(type_f());
//}

deleteFileDirAction_c::deleteFileDirAction_c(
        const actionData_c& actionData_par_con
        , const deleteFileDirData_c& deleteFileDirData_par_con)
    : action_c(actionData_par_con)
    , deleteFileDirData_c(deleteFileDirData_par_con)
{
}

void deleteFileDirAction_c::updateDeleteFileDirData_f(const deleteFileDirData_c& deleteFileDirData_par_con)
{
    this->deleteFileDirData_c::operator=(deleteFileDirData_par_con);
}

