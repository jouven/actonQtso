#include "sameFile.hpp"

#include "../checkExecution/sameFileExecution.hpp"
#include "../checkMappings/checkStrMapping.hpp"
#include "../actonDataHub.hpp"
#include "../reused/stringAlgo.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"

#include <QJsonObject>

QString sameFileData_c::fileAPath_f() const
{
    return fileAPath_pro;
}

QString sameFileCheck_c::fileAPathParsed_f() const
{
    return stringParserMap_c::parseString_f(fileAPath_pro, parentAction_f()->actonDataHubParent_f()->executionOptions_f().stringParserMap_f());
}

void sameFileData_c::setFileAPath_f(const QString& fileAPath_par_con)
{
    fileAPath_pro = fileAPath_par_con;
}

QString sameFileData_c::fileBPath_f() const
{
    return fileBPath_pro;
}

QString sameFileCheck_c::fileBPathParsed_f() const
{
    return stringParserMap_c::parseString_f(fileBPath_pro, parentAction_f()->actonDataHubParent_f()->executionOptions_f().stringParserMap_f());
}

void sameFileData_c::setFileBPath_f(const QString& fileBPath_par_con)
{
    fileBPath_pro = fileBPath_par_con;
}

bool sameFileData_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool isValidResultTmp(false);
    while (true)
    {
        if (fileAPath_f().isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "File A path is empty");
            break;
        }

        {
            text_c errorTextTmp;
            if (isValidStringSize_f(fileAPath_f(), 255, std::addressof(errorTextTmp), "File A path is too long: {0} (maximum length is {1})"))
            {
                //it's valid
            }
            else
            {
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
                break;
            }
        }

        if (fileBPath_f().isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "File B path is empty");
            break;
        }

        {
            text_c errorTextTmp;
            if (isValidStringSize_f(fileBPath_f(), 255, std::addressof(errorTextTmp), "File B path is too long: {0} (maximum length is {1})"))
            {
                //it's valid
            }
            else
            {
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
                break;
            }
        }

        isValidResultTmp = true;
        break;
    }
    return isValidResultTmp;
}

sameFileData_c::sameFileData_c(
        const QString& fileAPath_par_con
        , const QString& fileBPath_par_con)
    : fileAPath_pro(fileAPath_par_con)
    , fileBPath_pro(fileBPath_par_con)
{}

void sameFileCheck_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["fileAPath"] = fileAPath_pro;
    json_par["fileBPath"] = fileBPath_pro;
}

void sameFileCheck_c::derivedRead_f(const QJsonObject& json_par_con)
{
    fileAPath_pro = json_par_con["fileAPath"].toString();
    fileBPath_pro = json_par_con["fileBPath"].toString();
}

bool sameFileCheck_c::derivedIsValidCheck_f(textCompilation_c* errors_par) const
{
    return isFieldsDataValid_f(errors_par);
}

check_c* sameFileCheck_c::derivedClone_f() const
{
    //slice and dice
    sameFileData_c sameFileDataTmp(*this);
    checkData_c checkDataTmp(*this);
    return new sameFileCheck_c(checkDataTmp, sameFileDataTmp);
}

baseCheckExecution_c* sameFileCheck_c::createExecutionObj_f(checkExecutionResult_c* checkDataExecutionResult_ptr_par)
{
    return new sameFileCheckExecution_c(checkDataExecutionResult_ptr_par, this);
}

checkType_ec sameFileCheck_c::type_f() const
{
    return checkType_ec::sameFile;
}

uint64_t sameFileCheck_c::derivedUpdateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con)
{
    uint_fast64_t resultTmp(0);
    resultTmp = resultTmp
            + replaceSubString_f(fileAPath_pro, oldStringTrigger_par_con, newStringTrigger_par_con)
            + replaceSubString_f(fileBPath_pro, oldStringTrigger_par_con, newStringTrigger_par_con);
    return resultTmp;
}

uint64_t sameFileCheck_c::derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const
{
    uint_fast64_t resultTmp(0);
    resultTmp = resultTmp
            + vectorQStringCountSubString_f(stringTrigger_par_con, {fileAPath_pro, fileBPath_pro});
    return resultTmp;
}

QSet<QString> sameFileCheck_c::derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const
{
    QSet<QString> resultTmp;
    for (const QString& searchValue_ite_con : searchValues_par_con)
    {
        if (vectorQStringCountSubString_f(searchValue_ite_con, {fileAPath_pro, fileBPath_pro}, true) > 0)
        {
            resultTmp.insert(searchValue_ite_con);
        }
    }
    return resultTmp;
}

QString sameFileCheck_c::derivedReference_f() const
{
    return "from_" + fileAPath_pro + "_to_" + fileBPath_pro;
}

sameFileCheck_c::sameFileCheck_c(
        const checkData_c& checkData_par_con
        , const sameFileData_c& sameFileData_par_con)
    : check_c(checkData_par_con)
    , sameFileData_c(sameFileData_par_con)
{}

void sameFileCheck_c::updateSameFileData_f(const sameFileData_c& sameFileData_par_con)
{
    this->sameFileData_c::operator=(sameFileData_par_con);
}

