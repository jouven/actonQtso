#ifndef ACTONQTSO_CREATEDIRECTORYEXECUTION_HPP
#define ACTONQTSO_CREATEDIRECTORYEXECUTION_HPP

#include "baseActionExecution.hpp"

class createDirectoryAction_c;

class createDirectoryActionExecution_c : public baseActionExecution_c
{
    Q_OBJECT

    const createDirectoryAction_c* const createDirectoryActionPtr_pri = nullptr;
protected:
    void derivedExecute_f() override;
    //does nothing
    void derivedStop_f() override;
    //does nothing
    void derivedKill_f() override;
public:
    createDirectoryActionExecution_c() = delete;
    explicit createDirectoryActionExecution_c(
            actionDataExecutionResult_c* actionExecutionResultObj_par_con
            , createDirectoryAction_c* createDirectoryActionPtr_par
    );
};

#endif // ACTONQTSO_CREATEDIRECTORYEXECUTION_HPP
