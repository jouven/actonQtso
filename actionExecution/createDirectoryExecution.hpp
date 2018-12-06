#ifndef ACTONQTSO_CREATEDIRECTORYEXECUTION_HPP
#define ACTONQTSO_CREATEDIRECTORYEXECUTION_HPP

#include "baseActionExecution.hpp"

#include "../actions/createDirectory.hpp"

class createDirectoryActionExecution_c : public baseActionExecution_c, public createDirectoryAction_c
{
    Q_OBJECT

public:
    createDirectoryActionExecution_c() = delete;
    explicit createDirectoryActionExecution_c(
            const createDirectoryAction_c& createDirectoryAction_par_con
    );

    void execute_f() override;
    //does nothing
    void stop_f() override;
    //does nothing
    void kill_f() override;
Q_SIGNALS:

private Q_SLOTS:
};

#endif // ACTONQTSO_CREATEDIRECTORYEXECUTION_HPP
