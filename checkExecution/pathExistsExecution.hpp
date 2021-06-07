#ifndef ACTONQTSO_PATHEXISTSEXECUTION_HPP
#define ACTONQTSO_PATHEXISTSEXECUTION_HPP

#include "baseCheckExecution.hpp"

class pathExistsCheck_c;

class pathExistsCheckExecution_c : public baseCheckExecution_c
{
    Q_OBJECT

    const pathExistsCheck_c* const pathExistsCheckPtr_pri = nullptr;
protected:
    void derivedExecute_f() override;
    void derivedStop_f() override;
public:
    pathExistsCheckExecution_c() = delete;
    explicit pathExistsCheckExecution_c(
            checkExecutionResult_c* checkExecutionResultObj_par_con
            , pathExistsCheck_c* pathExistsCheckPtr_par
    );

};

#endif // ACTONQTSO_PATHEXISTSEXECUTION_HPP
