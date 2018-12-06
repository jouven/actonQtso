#ifndef ACTONQTSO_SAMEFILEEXECUTION_HPP
#define ACTONQTSO_SAMEFILEEXECUTION_HPP

#include "baseCheckExecution.hpp"

#include "../checks/sameFile.hpp"

class sameFileCheckExecution_c : public baseCheckExecution_c, public sameFileCheck_c
{
    Q_OBJECT

public:
    sameFileCheckExecution_c() = delete;
    explicit sameFileCheckExecution_c(
            const sameFileCheck_c& samefileCheck_par_con
    );

    void execute_f() override;
//Q_SIGNALS:

};

#endif // ACTONQTSO_SAMEFILEEXECUTION_HPP
