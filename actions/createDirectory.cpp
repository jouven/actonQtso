#include "createDirectory.hpp"

#include "../actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

QString createDirectoryAction_c::directoryPath_f() const
{
    return directoryPath_pri;
}

QString createDirectoryAction_c::directoryPathParsed_f() const
{
    COPYPARSERETURNVAR(directoryPath_pri);
}

void createDirectoryAction_c::setDirectoryPath_f(const QString& directoryPath_par_con)
{
    directoryPath_pri = directoryPath_par_con;
}

bool createDirectoryAction_c::createParents_f() const
{
    return createParents_pri;
}

void createDirectoryAction_c::setCreateParents_f(const bool createParents_par_con)
{
    createParents_pri = createParents_par_con;
}

bool createDirectoryAction_c::errorIfExists_f() const
{
    return errorIfExists_pri;
}

void createDirectoryAction_c::setErrorIfExists_f(const bool errorIfExists_par_con)
{
    errorIfExists_pri = errorIfExists_par_con;
}

createDirectoryAction_c::createDirectoryAction_c(
        const QString& directoryPath_par_con
        , const bool createParents_par_con
        , const bool errorIfExists_par_con)
    : directoryPath_pri(directoryPath_par_con)
    , createParents_pri(createParents_par_con)
    , errorIfExists_pri(errorIfExists_par_con)
{}

void createDirectoryAction_c::write_f(QJsonObject& json_par) const
{
    json_par["directoryPath"] = directoryPath_pri;
    json_par["createParents"] = createParents_pri;
    json_par["errorIfExists"] = errorIfExists_pri;
}

void createDirectoryAction_c::read_f(const QJsonObject& json_par_con)
{
    directoryPath_pri = json_par_con["directoryPath"].toString();
    createParents_pri = json_par_con["createParents"].toBool();
    errorIfExists_pri = json_par_con["errorIfExists"].toBool();
}
