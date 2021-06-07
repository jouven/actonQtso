#include "pathExists.hpp"

#include "../checkExecution/pathExistsExecution.hpp"
#include "../checkMappings/checkStrMapping.hpp"
#include "../actonDataHub.hpp"
#include "../reused/stringAlgo.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "essentialQtso/macros.hpp"
#include "textQtso/text.hpp"

#include <QJsonObject>

QString pathExistsData_c::path_f() const
{
    return path_pro;
}

QString pathExistsCheck_c::pathParsed_f() const
{
    return stringParserMap_c::parseString_f(path_pro, parentAction_f()->actonDataHubParent_f()->executionOptions_f().stringParserMap_f());
}

void pathExistsData_c::setPath_f(const QString& path_par_con)
{
    path_pro = path_par_con;
}

bool pathExistsData_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
        text_c errorTextTmp;
        if (isValidStringSize_f(path_f(), 255, std::addressof(errorTextTmp), "Path is too long: {0} (maximum length is {1})"))
        {
            //it's valid
        }
        else
        {
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
            break;
        }
        if (path_pro.isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Path is empty")
            break;
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}

pathExistsData_c::pathExistsData_c(
        const QString& path_par_con
        )
    : path_pro(path_par_con)
{}

void pathExistsCheck_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["path"] = path_pro;
}

void pathExistsCheck_c::derivedRead_f(const QJsonObject& json_par_con)
{
    path_pro = json_par_con["path"].toString();
}

bool pathExistsCheck_c::derivedIsValidCheck_f(textCompilation_c* errors_par) const
{
    return isFieldsDataValid_f(errors_par);
}

check_c* pathExistsCheck_c::derivedClone_f() const
{
    //slice and dice
    pathExistsData_c sameFileDataTmp(*this);
    checkData_c checkDataTmp(*this);
    return new pathExistsCheck_c(checkDataTmp, sameFileDataTmp);
}

baseCheckExecution_c* pathExistsCheck_c::createExecutionObj_f(checkExecutionResult_c* checkDataExecutionResult_ptr_par)
{
    return new pathExistsCheckExecution_c(checkDataExecutionResult_ptr_par, this);
}

checkType_ec pathExistsCheck_c::type_f() const
{
    return checkType_ec::pathExists;
}

uint64_t pathExistsCheck_c::derivedUpdateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con)
{
    uint_fast64_t resultTmp(0);
    resultTmp = resultTmp + replaceSubString_f(path_pro, oldStringTrigger_par_con, newStringTrigger_par_con);
    return resultTmp;
}

uint64_t pathExistsCheck_c::derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const
{
    uint_fast64_t resultTmp(0);
    resultTmp = resultTmp + vectorQStringCountSubString_f(path_pro, {stringTrigger_par_con});
    return resultTmp;
}

QSet<QString> pathExistsCheck_c::derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const
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

QString pathExistsCheck_c::derivedReference_f() const
{
    return path_pro;
}

pathExistsCheck_c::pathExistsCheck_c(
        const checkData_c& checkData_par_con
        , const pathExistsData_c& pathExistsData_par_con)
    : check_c(checkData_par_con)
    , pathExistsData_c(pathExistsData_par_con)
{}

void pathExistsCheck_c::updatePathExistsData_f(const pathExistsData_c& pathExistsData_par_con)
{
    this->pathExistsData_c::operator=(pathExistsData_par_con);
}

