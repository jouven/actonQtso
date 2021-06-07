#ifndef ACTONQTSO_SAMEFILEEXECUTION_HPP
#define ACTONQTSO_SAMEFILEEXECUTION_HPP

#include "baseCheckExecution.hpp"

#include "../reused/sameFileAlgo.hpp"

class sameFileCheck_c;

class sameFileCheckExecution_c : public baseCheckExecution_c
{
    Q_OBJECT

    const sameFileCheck_c* const sameFileCheckPtr_pri = nullptr;
    checkSameFile_c checkSameFile_pri;
protected:
    void derivedExecute_f() override;
    void derivedStop_f() override;
public:
    sameFileCheckExecution_c() = delete;
    explicit sameFileCheckExecution_c(
            checkExecutionResult_c* checkExecutionResultObj_par_con
            , sameFileCheck_c* samefileCheckPtr_par
    );
private Q_SLOTS:
    void sameFileErrorToExecutionMessage_f(const text_c& error_par);
};

#endif // ACTONQTSO_SAMEFILEEXECUTION_HPP
