#ifndef ACTONQTSO_DELETEFILEDIR_HPP
#define ACTONQTSO_DELETEFILEDIR_HPP

#include "../actionData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>

class QJsonObject;
class textCompilation_c;

class EXPIMP_ACTONQTSO deleteFileDirData_c
{
protected:
    //this can be absolute or relative
    QString path_pro;
    bool errorIfNotExists_pro = false;
    //directories: if there are no other files/directories inside
    //files: if they are 0 sized
    //otherwise they are ignored (no error)
    bool onlyIfEmpty_pro = false;

    //FUTURE add, right now it only deletes a single directorie or a single file
    //directoryFilter_c* directoryFilterPtr_pro = nullptr;
    //prevent public assignments
    deleteFileDirData_c& operator=(deleteFileDirData_c const &) = default;
    deleteFileDirData_c& operator=(deleteFileDirData_c&) = default;
    deleteFileDirData_c& operator=(deleteFileDirData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    deleteFileDirData_c(deleteFileDirData_c const&) = default;
    deleteFileDirData_c(deleteFileDirData_c&) = default;

    deleteFileDirData_c() = default;
    deleteFileDirData_c(
            const QString& path_par_con
            , const bool errorIfNotExists_par_con
            , const bool onlyIfEmpty_par_con
    );

    QString path_f() const;
    QString pathParsed_f() const;

    void setPath_f(const QString& path_par_con);
    bool errorIfNotExists_f() const;
    void setErrorIfNotExists_f(const bool errorIfNotExists_par_con);
    bool onlyIfEmpty_f() const;
    void setOnlyIfEmpty_f(const bool onlyIfEmpty_par_con);

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};

class EXPIMP_ACTONQTSO deleteFileDirAction_c : public action_c, public deleteFileDirData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidAction_f(textCompilation_c* errors_par = nullptr) const override;

    action_c* derivedClone_f() const override;

    baseActionExecution_c* createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par) override;
    actionType_ec type_f() const override;
    QString typeStr_f() const override;
public:
    deleteFileDirAction_c() = default;
    deleteFileDirAction_c(const actionData_c& actionData_par_con, const deleteFileDirData_c& deleteFileDirData_par_con);

    void updateDeleteFileDirData_f(const deleteFileDirData_c& deleteFileDirData_par_con);
};

#endif // ACTONQTSO_CREATEDIRECTORY_HPP
