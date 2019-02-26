#ifndef ACTONQTSO_SAMEFILEEXECUTION_HPP
#define ACTONQTSO_SAMEFILEEXECUTION_HPP

#include "baseCheckExecution.hpp"

#include "../checks/sameFile.hpp"
#include "../reused/sameFileAlgo.hpp"

class sameFileCheckExecution_c : public baseCheckExecution_c, public sameFileCheck_c
{
    Q_OBJECT

    checkSameFile_c checkSameFile_pri;
protected:
    void derivedExecute_f() override;
    void derivedStop_f() override;
public:
public:
    sameFileCheckExecution_c() = delete;
    explicit sameFileCheckExecution_c(
            checkDataExecutionResult_c* checkExecutionResultObj_par_con
            , const sameFileCheck_c& samefileCheck_par_con
    );

};

#endif // ACTONQTSO_SAMEFILEEXECUTION_HPP
