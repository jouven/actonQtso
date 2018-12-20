#ifndef ACTONQTSO_CREATEDIRECTORYEXECUTION_HPP
#define ACTONQTSO_CREATEDIRECTORYEXECUTION_HPP

#include "baseActionExecution.hpp"

#include "../actions/createDirectory.hpp"

class createDirectoryActionExecution_c : public baseActionExecution_c, public createDirectoryAction_c
{
    Q_OBJECT
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
            , const createDirectoryAction_c& createDirectoryAction_par_con
    );
};

#endif // ACTONQTSO_CREATEDIRECTORYEXECUTION_HPP
