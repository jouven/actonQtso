#include "createDirectory.hpp"

#include "../actionExecution/createDirectoryExecution.hpp"
#include "../actionMappings/actionStrMapping.hpp"
#include "../actonDataHub.hpp"

#include "essentialQtso/macros.hpp"
#include "textQtso/text.hpp"
#include "stringParserMapQtso/stringParserMap.hpp"

#include <QJsonObject>

QString createDirectoryData_c::directoryPath_f() const
{
    return directoryPath_pro;
}

QString createDirectoryData_c::directoryPathParsed_f() const
{
    COPYPARSERETURNVAR(directoryPath_pro);
}

void createDirectoryData_c::setDirectoryPath_f(const QString& directoryPath_par_con)
{
    directoryPath_pro = directoryPath_par_con;
}

bool createDirectoryData_c::createParents_f() const
{
    return createParents_pro;
}

void createDirectoryData_c::setCreateParents_f(const bool createParents_par_con)
{
    createParents_pro = createParents_par_con;
}

bool createDirectoryData_c::errorIfExists_f() const
{
    return errorIfExists_pro;
}

void createDirectoryData_c::setErrorIfExists_f(const bool errorIfExists_par_con)
{
    errorIfExists_pro = errorIfExists_par_con;
}


bool createDirectoryData_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
        text_c errorTextTmp;
        if (isValidStringSize_f(directoryPathParsed_f(), 255, std::addressof(errorTextTmp), "Source path is too long: {0} (maximum length is {1})"))
        {
            //it's valid
        }
        else
        {
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
            break;
        }
        if (directoryPath_pro.isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Directory path is empty")
            break;
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}


createDirectoryData_c::createDirectoryData_c(
        const QString& directoryPath_par_con
        , const bool createParents_par_con
        , const bool errorIfExists_par_con)
    : directoryPath_pro(directoryPath_par_con)
    , createParents_pro(createParents_par_con)
    , errorIfExists_pro(errorIfExists_par_con)
{}

void createDirectoryAction_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["directoryPath"] = directoryPath_pro;
    json_par["createParents"] = createParents_pro;
    json_par["errorIfExists"] = errorIfExists_pro;
}

void createDirectoryAction_c::derivedRead_f(const QJsonObject& json_par_con)
{
    directoryPath_pro = json_par_con["directoryPath"].toString();
    if (json_par_con["createParents"].isBool())
    {
        createParents_pro = json_par_con["createParents"].toBool();
    }
    if (json_par_con["errorIfExists"].isBool())
    {
        errorIfExists_pro = json_par_con["errorIfExists"].toBool();
    }
}

bool createDirectoryAction_c::derivedIsValidAction_f(textCompilation_c* errors_par) const
{
    return isFieldsDataValid_f(errors_par);
}

action_c* createDirectoryAction_c::derivedClone_f() const
{
    //slice and dice
    createDirectoryData_c createDirectoryDataTmp(*this);
    actionData_c actionDataTmp(*this);
    return new createDirectoryAction_c(actionDataTmp, createDirectoryDataTmp);
}

baseActionExecution_c* createDirectoryAction_c::createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par)
{
    return new createDirectoryActionExecution_c(actionDataExecutionResult_ptr_par, this);
}

actionType_ec createDirectoryAction_c::type_f() const
{
    return actionType_ec::createDirectory;
}

QString createDirectoryAction_c::typeStr_f() const
{
    return actionTypeToStrUMap_ext_con.at(type_f());
}

createDirectoryAction_c::createDirectoryAction_c(
        const actionData_c& actionData_par_con
        , const createDirectoryData_c& createDirectoryData_par_con)
    : action_c(actionData_par_con)
    , createDirectoryData_c(createDirectoryData_par_con)
{
}

void createDirectoryAction_c::updateCreateDirectoryData_f(const createDirectoryData_c& createDirectoryData_par_con)
{
    this->createDirectoryData_c::operator=(createDirectoryData_par_con);
}

