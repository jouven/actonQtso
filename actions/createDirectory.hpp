#ifndef ACTONQTSO_CREATEDIRECTORY_HPP
#define ACTONQTSO_CREATEDIRECTORY_HPP

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QJsonObject>

class EXPIMP_ACTONQTSO createDirectoryAction_c
{
    //this can be absolute or relative
    QString directoryPath_pri;
    bool createParents_pri = true;
    bool errorIfExists_pri = false;

public:
    createDirectoryAction_c() = default;
    createDirectoryAction_c(
            const QString& directoryPath_par_con
            , const bool createParents_par_con
            , const bool errorIfExists_par_con
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString directoryPath_f() const;
    void setDirectoryPath_f(const QString& directoryPath_par_con);
    bool createParents_f() const;
    void setCreateParents_f(const bool createParents_par_con);
    bool errorIfExists_f() const;
    void setErrorIfExists_f(const bool errorIfExists_par_con);
};

#endif // ACTONQTSO_CREATEDIRECTORY_HPP
