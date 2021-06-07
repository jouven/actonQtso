#ifndef ACTONQTSO_SAMEFILE_HPP
#define ACTONQTSO_SAMEFILE_HPP

#include "../checkData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>

class QJsonObject;
class textCompilation_c;

//FUTURE buffersize setting
class EXPIMP_ACTONQTSO sameFileData_c
{
protected:
    //these can be absolute or relative
    QString fileAPath_pro;
    QString fileBPath_pro;

    //prevent public assignments
    sameFileData_c& operator=(sameFileData_c const &) = default;
    sameFileData_c& operator=(sameFileData_c&) = default;
    sameFileData_c& operator=(sameFileData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    sameFileData_c(sameFileData_c const&) = default;
    sameFileData_c(sameFileData_c&) = default;

    sameFileData_c() = default;
    explicit sameFileData_c(
            const QString& fileAPath_par_con
            , const QString& fileBPath_par_con
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString fileAPath_f() const;

    void setFileAPath_f(const QString& fileAPath_par_con);
    QString fileBPath_f() const;

    void setFileBPath_f(const QString& fileBPath_par_con);

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};


class EXPIMP_ACTONQTSO sameFileCheck_c : public check_c, public sameFileData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidCheck_f(textCompilation_c* errors_par = nullptr) const override;

    check_c* derivedClone_f() const override;

    baseCheckExecution_c* createExecutionObj_f(checkExecutionResult_c* checkDataExecutionResult_ptr_par) override;
    checkType_ec type_f() const override;

    //uint_fast64_t derivedStringTriggerCreationConflictCount_f(const QString& stringTrigger_par_con) const override;
    //uint64_t derivedUpdateActionStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con) override;
    //uint64_t derivedActionStringIdDependencyCount_f(const QString& stringId_par_con) const override;
    uint64_t derivedUpdateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con) override;
    uint64_t derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const override;
    QSet<QString> derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const override;
    //QSet<QString> derivedStringTriggerCreationCollection_f() const override;

    QString derivedReference_f() const override;
public:
    sameFileCheck_c() = default;
    sameFileCheck_c(const checkData_c& checkData_par_con, const sameFileData_c& sameFileData_par_con);

    void updateSameFileData_f(const sameFileData_c& sameFileData_par_con);

    QString fileAPathParsed_f() const;
    QString fileBPathParsed_f() const;
};

#endif // ACTONQTSO_SAMEFILE_HPP
