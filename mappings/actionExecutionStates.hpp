#ifndef ACTONQTG_ACTIONEXECUTIONSTATES_HPP
#define ACTONQTG_ACTIONEXECUTIONSTATES_HPP

//shared by all types of actions
enum class actionExecutionState_ec
{
    initial = 0
    , running = 1
    , error = 2
    , stoppedByUser = 3
    , timeOut = 4
    , success = 5
};

#endif // ACTONQTG_ACTIONEXECUTIONSTATES_HPP
