#ifndef ACTONQTSO_RUNPROCESS_HPP
#define ACTONQTSO_RUNPROCESS_HPP

#include "../actionData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QDir>
#include <QHash>

#include <vector>

class QJsonObject;
class textCompilation_c;

class EXPIMP_ACTONQTSO argument_c
{
    QString argument_pri;
    bool enabled_pri = true;
public:
    argument_c() = default;
    explicit argument_c(
            const QString& argument_par_con
            , const bool enabled_par_con = true
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString argument_f() const;
    QString argumentParsed_f() const;
    void setArgument_f(const QString& value_par_con);
    bool enabled_f() const;
    void setEnabled_f(const bool enabled_par_con);
};


//the key part is located in the QHash key (runProcessAction_c.environmentToAdd_pri)
class EXPIMP_ACTONQTSO environmentPairConfig_c
{
    QString environmentValue_pri;
    bool enabled_pri = true;
public:
    environmentPairConfig_c() = default;
    explicit environmentPairConfig_c(
            const QString& environmentValue_par_con
            , const bool enabled_par_con = true
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString environmentValue_f() const;
    QString environmentValueParsed_f() const;
    void setEnvironmentValue_f(const QString& environmentValue_par_con);

    bool enabled_f() const;
    void setEnabled_f(const bool enabled_par_con);
};

class EXPIMP_ACTONQTSO runProcessData_c
{
protected:
    QString processPath_pro;
    std::vector<argument_c> arguments_pro;
    //this action type has a working directory because QProcess has the option
    QString workingDirectory_pro;
    //enviroment requires some extra options

    //copy the main program (i.e. actonQtg) environment
    //when executing the QProcess (see runProcessActionExecution_c class)
    //otherwise use an empty one
    bool useProgramEnvironment_pro = true;
    //specific environment to add to the "base" one
    QHash<QString, environmentPairConfig_c> environmentToAdd_pro;

    //prevent public assignments
    runProcessData_c& operator=(runProcessData_c const &) = default;
    runProcessData_c& operator=(runProcessData_c&) = default;
    runProcessData_c& operator=(runProcessData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    runProcessData_c(runProcessData_c const&) = default;
    runProcessData_c(runProcessData_c&) = default;

    runProcessData_c() = default;
    explicit runProcessData_c(
            const QString& processPath_par_con
            , const std::vector<argument_c>& arguments_par_con = std::vector<argument_c>()
            , const QString& workingDirectory_par_con = QDir::currentPath()
            , const bool useProcessEnvironment_par_con = true
            , const QHash<QString, environmentPairConfig_c>& environmentToAdd_par_con = QHash<QString, environmentPairConfig_c>()
    );

    QString processPath_f() const;
    QString processPathParsed_f() const;
    void setProcessPath_f(const QString& processPath_par_con);
    std::vector<argument_c> arguments_f() const;
    std::vector<argument_c> argumentsParsed_f() const;
    void setArguments_f(const std::vector<argument_c>& arguments_par_con);
    QString workingDirectory_f() const;
    QString workingDirectoryParsed_f() const;
    void setWorkingDirectory_f(const QString& workingDirectory_par_con);
    QHash<QString, environmentPairConfig_c> environmentToAdd_f() const;
    QHash<QString, environmentPairConfig_c> environmentToAddParsed_f() const;
    void setEnvironmentToAdd_f(const QHash<QString, environmentPairConfig_c>& environmentToAdd_par_con);
    bool useProgramEnvironment_f() const;
    void setUseProgramEnvironment_f(const bool useProgramEnvironment_par_con);

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};

class EXPIMP_ACTONQTSO runProcessAction_c : public action_c, public runProcessData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidAction_f(textCompilation_c* errors_par = nullptr) const override;

    //uint_fast64_t derivedUpdateActionStringIdDependencies_f(const QString& , const QString& ) override;
    //uint_fast64_t derivedActionStringIdDependencyCount_f(const QString& ) const override;
    //uint_fast64_t derivedStringTriggerCreationConflictCount_f(const QString& stringTrigger_par_con) const override;
    uint_fast64_t derivedUpdateStringTriggerDependecies_f(const QString& oldStringTrigger_par_con, const QString& newStringTrigger_par_con) override;
    uint_fast64_t derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const override;
    //QSet<QString> derivedStringTriggerCreationCollection_f() const override;
    QSet<QString> derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const override;

    action_c* derivedClone_f() const override;

    baseActionExecution_c* createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par) override;
    actionType_ec type_f() const override;
    //QString typeStr_f() const override;
public:
    runProcessAction_c() = default;
    runProcessAction_c(const actionData_c& actionData_par_con, const runProcessData_c& runProcessData_par_con);

    void updateRunProcessData_f(const runProcessData_c& runProcessData_par_con);
};

#endif // ACTONQTSO_RUNPROCESS_HPP
