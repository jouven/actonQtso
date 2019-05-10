#ifndef ACTONQTSO_RUNPROCESS_HPP
#define ACTONQTSO_RUNPROCESS_HPP

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QJsonObject>
#include <QDir>
#include <QHash>

#include <vector>

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

class EXPIMP_ACTONQTSO runProcessAction_c
{
    QString processPath_pri;
    std::vector<argument_c> arguments_pri;
    QString workingDirectory_pri;
    //enviroment requires some extra options

    //copy acton environment as a base
    //otherwise use an empty one
    bool useActonEnvironment_pri = true;
    //specific environment to add to the "base" one
    //keys must be unique
    QHash<QString, environmentPairConfig_c> environmentToAdd_pri;
public:
    runProcessAction_c() = default;
    runProcessAction_c(
            const QString& processPath_par_con
            , const std::vector<argument_c>& arguments_par_con = std::vector<argument_c>()
            , const QString& workingDirectory_par_con = QDir::currentPath()
            , const bool useProcessEnvironment_par_con = true
            , const QHash<QString, environmentPairConfig_c>& environmentToAdd_par_con = QHash<QString, environmentPairConfig_c>()
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

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

#endif // ACTONQTSO_RUNPROCESS_HPP
