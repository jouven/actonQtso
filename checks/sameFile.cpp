#include "sameFile.hpp"


QString sameFileCheck_c::fileAPath_f() const
{
    return fileAPath_pri;
}

void sameFileCheck_c::setFileAPath_f(const QString& fileAPath_par_con)
{
    fileAPath_pri = fileAPath_par_con;
}

QString sameFileCheck_c::fileBPath_f() const
{
    return fileBPath_pri;
}

void sameFileCheck_c::setFileBPath_f(const QString& fileBPath_par_con)
{
    fileBPath_pri = fileBPath_par_con;
}

sameFileCheck_c::sameFileCheck_c(
        const QString& fileAPath_par_con
        , const QString& fileBPath_par_con)
    : fileAPath_pri(fileAPath_par_con)
    , fileBPath_pri(fileBPath_par_con)
{}

void sameFileCheck_c::write_f(QJsonObject& json_par) const
{
    json_par["fileAPath"] = fileAPath_pri;
    json_par["fileBPath"] = fileBPath_pri;
}

void sameFileCheck_c::read_f(const QJsonObject& json_par_con)
{
    fileAPath_pri = json_par_con["fileAPath"].toString();
    fileBPath_pri = json_par_con["fileBPath"].toString();
}
