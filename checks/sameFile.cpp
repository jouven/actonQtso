#include "sameFile.hpp"

#include "../checkExecution/sameFileExecution.hpp"
#include "../checkMappings/checkStrMapping.hpp"
#include "../actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include <QJsonObject>

QString sameFileData_c::fileAPath_f() const
{
    return fileAPath_pro;
}

QString sameFileData_c::fileAPathParsed_f() const
{
    COPYPARSERETURNVAR(fileAPath_pro);
}

void sameFileData_c::setFileAPath_f(const QString& fileAPath_par_con)
{
    fileAPath_pro = fileAPath_par_con;
}

QString sameFileData_c::fileBPath_f() const
{
    return fileBPath_pro;
}

QString sameFileData_c::fileBPathParsed_f() const
{
    COPYPARSERETURNVAR(fileBPath_pro);
}

void sameFileData_c::setFileBPath_f(const QString& fileBPath_par_con)
{
    fileBPath_pro = fileBPath_par_con;
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

check_c* sameFileCheck_c::derivedClone_f() const
{
    //slice and dice
    sameFileData_c sameFileDataTmp(*this);
    checkData_c checkDataTmp(*this);
    return new sameFileCheck_c(checkDataTmp, sameFileDataTmp);
}

baseCheckExecution_c* sameFileCheck_c::createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par)
{
    return new sameFileCheckExecution_c(checkDataExecutionResult_ptr_par, this);
}

checkType_ec sameFileCheck_c::type_f() const
{
    return checkType_ec::sameFile;
}

QString sameFileCheck_c::typeStr_f() const
{
    return checkTypeToStrUMap_glo_sta_con.at(checkType_ec::sameFile);
}

sameFileCheck_c::sameFileCheck_c(
        const checkData_c& checkData_par_con
        , const sameFileData_c& sameFile_par_con)
    : check_c(checkData_par_con)
    , sameFileData_c(sameFile_par_con)
{

}

