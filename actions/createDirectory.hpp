#ifndef ACTONQTSO_CREATEDIRECTORY_HPP
#define ACTONQTSO_CREATEDIRECTORY_HPP

#include "../actionData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>

class QJsonObject;
class textCompilation_c;

class EXPIMP_ACTONQTSO createDirectoryData_c
{
protected:
    //this can be absolute or relative
    QString directoryPath_pro;
    bool createParents_pro = true;
    bool errorIfExists_pro = false;

    //prevent public assignments
    createDirectoryData_c& operator=(createDirectoryData_c const &) = default;
    createDirectoryData_c& operator=(createDirectoryData_c&) = default;
    createDirectoryData_c& operator=(createDirectoryData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    createDirectoryData_c(createDirectoryData_c const&) = default;
    createDirectoryData_c(createDirectoryData_c&) = default;

    createDirectoryData_c() = default;
    explicit createDirectoryData_c(
            const QString& directoryPath_par_con
            , const bool createParents_par_con
            , const bool errorIfExists_par_con
    );

    QString directoryPath_f() const;

    void setDirectoryPath_f(const QString& directoryPath_par_con);
    bool createParents_f() const;
    void setCreateParents_f(const bool createParents_par_con);
    bool errorIfExists_f() const;
    void setErrorIfExists_f(const bool errorIfExists_par_con);

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};

class EXPIMP_ACTONQTSO createDirectoryAction_c : public action_c, public createDirectoryData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidAction_f(textCompilation_c* errors_par = nullptr) const override;

    //uint_fast64_t derivedUpdateActionStringIdDependencies_f(const QString& , const QString& ) override;
    //uint_fast64_t derivedActionStringIdDependencyCount_f(const QString& ) const override;
    //uint_fast64_t derivedStringTriggerCreationConflictCount_f(const QString& ) const override;
    uint_fast64_t derivedUpdateStringTriggerDependecies_f(const QString& oldStringTrigger_par_con, const QString& newStringTrigger_par_con) override;
    uint_fast64_t derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const override;
    //QSet<QString> derivedStringTriggerCreationCollection_f() const override;
    QSet<QString> derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const override;

    action_c* derivedClone_f() const override;

    baseActionExecution_c* createExecutionObj_f(actionExecutionResult_c* actionDataExecutionResult_ptr_par) override;
    actionType_ec type_f() const override;
    //QString typeStr_f() const override;

    QString derivedReference_f() const override;
public:
    createDirectoryAction_c() = default;
    createDirectoryAction_c(actonDataHub_c* actonDataHubParent_par, const actionData_c& actionData_par_con, const createDirectoryData_c& createDirectoryData_par_con);

    QString directoryPathParsed_f() const;

    void updateCreateDirectoryData_f(const createDirectoryData_c& createDirectoryData_par_con);
};

#endif // ACTONQTSO_CREATEDIRECTORY_HPP
