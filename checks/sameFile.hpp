#ifndef ACTONQTSO_SAMEFILE_HPP
#define ACTONQTSO_SAMEFILE_HPP

#include "../checkData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>

class QJsonObject;

//FUTURE buffersize setting
class EXPIMP_ACTONQTSO sameFileData_c
{
protected:
    //these can be absolute or relative
    QString fileAPath_pro;
    QString fileBPath_pro;

public:
    sameFileData_c() = default;
    sameFileData_c(
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


class EXPIMP_ACTONQTSO sameFileCheck_c : public check_c, public sameFileData_c
{
    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;

    check_c* derivedClone_f() const override;

    baseCheckExecution_c* createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par) override;
    checkType_ec type_f() const override;
    QString typeStr_f() const override;
public:
    sameFileCheck_c() = default;
    sameFileCheck_c(const checkData_c& checkData_par_con, const sameFileData_c& sameFile_par_con);
};

#endif // ACTONQTSO_SAMEFILE_HPP
