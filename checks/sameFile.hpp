#ifndef ACTONQTSO_SAMEFILE_HPP
#define ACTONQTSO_SAMEFILE_HPP

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QJsonObject>

//FUTURE buffersize setting
class EXPIMP_ACTONQTSO sameFileCheck_c
{
    //these can be absolute or relative
    QString fileAPath_pri;
    QString fileBPath_pri;

public:
    sameFileCheck_c() = default;
    sameFileCheck_c(
            const QString& fileAPath_par_con
            , const QString& fileBPath_par_con
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString fileAPath_f() const;
    QString fileAPathParsed_f() const;
    void setFileAPath_f(const QString& fileAPath_par_con);
    QString fileBPath_f() const;
    QString fileBPathParsed_f() const;
    void setFileBPath_f(const QString& fileBPath_par_con);
};

#endif // ACTONQTSO_SAMEFILE_HPP
