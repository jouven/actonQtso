#ifndef ACTONQTSO_PATHEXISTS_HPP
#define ACTONQTSO_PATHEXISTS_HPP

#include "../checkData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>

class QJsonObject;
class textCompilation_c;

class EXPIMP_ACTONQTSO pathExistsData_c
{
protected:
    //can be absolute or relative
    QString path_pro;

    //prevent public assignments
    pathExistsData_c& operator=(pathExistsData_c const &) = default;
    pathExistsData_c& operator=(pathExistsData_c&) = default;
    pathExistsData_c& operator=(pathExistsData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    pathExistsData_c(pathExistsData_c const&) = default;
    pathExistsData_c(pathExistsData_c&) = default;

    pathExistsData_c() = default;
    explicit pathExistsData_c(
            const QString& path_par_con
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString path_f() const;

    void setPath_f(const QString& path_par_con);

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};


class EXPIMP_ACTONQTSO pathExistsCheck_c : public check_c, public pathExistsData_c
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
    pathExistsCheck_c() = default;
    pathExistsCheck_c(const checkData_c& checkData_par_con, const pathExistsData_c& pathExistsData_par_con);

    void updatePathExistsData_f(const pathExistsData_c& pathExistsData_par_con);

    QString pathParsed_f() const;
};

#endif // ACTONQTSO_PATHEXISTS_HPP
