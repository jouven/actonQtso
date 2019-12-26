#ifndef ACTONQTSO_DELETEFILEDIREXECUTION_HPP
#define ACTONQTSO_DELETEFILEDIREXECUTION_HPP

#include "baseActionExecution.hpp"

class deleteFileDirAction_c;

class deleteFiledirActionExecution_c : public baseActionExecution_c
{
    Q_OBJECT

    const deleteFileDirAction_c* const deleteFileDirActionPtr_pri = nullptr;
protected:
    void derivedExecute_f() override;
    //does nothing
    void derivedStop_f() override;
    //does nothing
    void derivedKill_f() override;
public:
    deleteFiledirActionExecution_c() = delete;
    explicit deleteFiledirActionExecution_c(
            actionDataExecutionResult_c* actionExecutionResultObj_par_con
            , deleteFileDirAction_c* deleteFileDirActionPtr_par
    );
};

#endif // ACTONQTSO_DELETEFILEDIREXECUTION_HPP
