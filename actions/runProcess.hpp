#ifndef ACTONQTSO_RUNPROCESS_HPP
#define ACTONQTSO_RUNPROCESS_HPP

#include <QString>
#include <QJsonObject>
#include <QDir>
#include <QHash>

#include <vector>

class argument_c
{
    QString value_pri;
    bool enabled_pri = true;
public:
    argument_c() = default;
    argument_c(const QString& value_par_con
            , const bool enabled_par_con = true
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString argument_f() const;
    void setArgument_f(const QString& value_par_con);
    bool enabled_f() const;
    void setEnabled_f(const bool enabled_par_con);
};


class environmentPair_c
{
    QString value_pri;
    bool enabled_pri = true;
public:
    environmentPair_c() = default;
    environmentPair_c(
            const QString& value_par_con
            , const bool enabled_par_con = true
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString value_f() const;
    void setValue_f(const QString& value_par_con);

    bool enabled_f() const;
    void setEnabled_f(const bool enabled_par_con);
};

class runProcessAction_c
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
    QHash<QString, environmentPair_c> environmentToAdd_pri;
public:
    runProcessAction_c() = default;
    runProcessAction_c(
            const QString& processPath_par_con
            , const std::vector<argument_c>& arguments_par_con = std::vector<argument_c>()
            , const QString& workingDirectory_par_con = QDir::currentPath()
            , const bool useProcessEnvironment_par_con = true
            , const QHash<QString, environmentPair_c>& environmentToAdd_par_con = QHash<QString, environmentPair_c>()
    );

    void write_f(QJsonObject &json_par) const;
    void read_f(const QJsonObject &json_par_con);

    QString processPath_f() const;
    void setProcessPath_f(const QString& processPath_par_con);
    std::vector<argument_c> arguments_f() const;
    void setArguments_f(const std::vector<argument_c>& arguments_par_con);
    QString workingDirectory_f() const;
    void setWorkingDirectory_f(const QString& workingDirectory_par_con);
    QHash<QString, environmentPair_c> environmentToAdd_f() const;
    void setEnvironmentToAdd_f(const QHash<QString, environmentPair_c>& environmentToAdd_par_con);
    bool useActonEnvironment_f() const;
    void setUseActonEnvironment_f(const bool useActonEnvironment_par_con);
};

#endif // ACTONQTSO_RUNPROCESS_HPP
