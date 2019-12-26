#include "pathExists.hpp"

#include "../checkExecution/pathExistsExecution.hpp"
#include "../checkMappings/checkStrMapping.hpp"
#include "../actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "essentialQtso/macros.hpp"
#include "textQtso/text.hpp"

#include <QJsonObject>

QString pathExistsData_c::path_f() const
{
    return path_pro;
}

QString pathExistsData_c::pathParsed_f() const
{
    COPYPARSERETURNVAR(path_pro);
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
        if (isValidStringSize_f(pathParsed_f(), 255, std::addressof(errorTextTmp), "Path is too long: {0} (maximum length is {1})"))
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

baseCheckExecution_c* pathExistsCheck_c::createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par)
{
    return new pathExistsCheckExecution_c(checkDataExecutionResult_ptr_par, this);
}

checkType_ec pathExistsCheck_c::type_f() const
{
    return checkType_ec::pathExists;
}

QString pathExistsCheck_c::typeStr_f() const
{
    return checkTypeToStrUMap_ext_con.at(type_f());
}

pathExistsCheck_c::pathExistsCheck_c(const checkData_c& checkData_par_con
        , const pathExistsData_c& pathExists_par_con)
    : check_c(checkData_par_con)
    , pathExistsData_c(pathExists_par_con)
{}

void pathExistsCheck_c::updatePathExistsData_f(const pathExistsData_c& pathExistsData_par_con)
{
    this->pathExistsData_c::operator=(pathExistsData_par_con);
}

