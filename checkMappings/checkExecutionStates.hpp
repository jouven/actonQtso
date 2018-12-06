#ifndef ACTONQTSO_CHECKEXECUTIONSTATES_HPP
#define ACTONQTSO_CHECKEXECUTIONSTATES_HPP

//shared by all types of checks
enum class checkExecutionState_ec
{
    initial = 0
    , preparing = 1
    , executing = 2
    , error = 3
    , stoppingByUser = 4
    , stoppedByUser = 5
    , timeOut = 6
    , finished = 7
};

#endif // ACTONQTSO_CHECKEXECUTIONSTATES_HPP
