#ifndef ACTONQTSO_RUNPROCESS_HPP
#define ACTONQTSO_RUNPROCESS_HPP

#include "../actionData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QDir>
#include <QHash>

#include <vector>

class QJsonObject;

class EXPIMP_ACTONQTSO argument_c
{
    QString argument_pri;
    bool enabled_pri = true;
public:
    argument_c() = default;
    argument_c(const QString& argument_par_con
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
    environmentPairConfig_c(
            const QString& environmentValue_par_con
            , const bool enabled_par_con = true
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString environmentValue() const;
    QString environmentValueParsed() const;
    void setEnvironmentValue_f(const QString& environmentValue_par_con);

    bool enabled_f() const;
    void setEnabled_f(const bool enabled_par_con);
};

class EXPIMP_ACTONQTSO runProcessData_c
{
protected:
    QString processPath_pro;
    std::vector<argument_c> arguments_pro;
    QString workingDirectory_pro;
    //enviroment requires some extra options

    //copy acton environment as a base
    //otherwise use an empty one
    bool useActonEnvironment_pro = true;
    //specific environment to add to the "base" one
    //keys must be unique
    QHash<QString, environmentPairConfig_c> environmentToAdd_pro;

public:
    runProcessData_c() = default;
    runProcessData_c(
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
    void setArguments_f(const std::vector<argument_c>& arguments_par_con);
    QString workingDirectory_f() const;
    QString workingDirectoryParsed_f() const;
    void setWorkingDirectory_f(const QString& workingDirectory_par_con);
    QHash<QString, environmentPairConfig_c> environmentToAdd_f() const;
    QHash<QString, environmentPairConfig_c> environmentToAddParsed_f() const;
    void setEnvironmentToAdd_f(const QHash<QString, environmentPairConfig_c>& environmentToAdd_par_con);
    bool useActonEnvironment_f() const;
    void setUseActonEnvironment_f(const bool useActonEnvironment_par_con);
};

class EXPIMP_ACTONQTSO runProcessAction_c : public action_c, public runProcessData_c
{
    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;

    action_c* derivedClone_f() const override;

    baseActionExecution_c* createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par) override;
    actionType_ec type_f() const override;
    QString typeStr_f() const override;
public:
    runProcessAction_c() = default;
    runProcessAction_c(const actionData_c& actionData_par_con, const runProcessData_c& runProcessData_par_con);

};

#endif // ACTONQTSO_RUNPROCESS_HPP
