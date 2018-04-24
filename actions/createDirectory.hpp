#ifndef ACTONQTG_CREATEDIRECTORY_HPP
#define ACTONQTG_CREATEDIRECTORY_HPP

#include <QString>
#include <QJsonObject>

class createDirectoryAction_c
{
    //this can be absolute or relative
    QString directoryPath_pri;
    bool createParents_pri = true;

public:
    createDirectoryAction_c() = default;
    createDirectoryAction_c(
            const QString& directoryPath_par_con
            , const bool createParents_par_con
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString directoryPath_f() const;
    void setDirectoryPath_f(const QString& directoryPath_par_con);
    bool createParents_f() const;
    void setCreateParents_f(const bool createParents_par_con);
};

#endif // ACTONQTG_CREATEDIRECTORY_HPP
