#ifndef ACTONQTSO_ACTIONEXECUTIONSTATES_HPP
#define ACTONQTSO_ACTIONEXECUTIONSTATES_HPP

//shared by all types of action executions
enum class actionExecutionState_ec
{
    //default state on action execution object creation
    initial = 0
    //executing checks
    , executingChecks = 1
    //when the qThread for the action starts but before the action really starts
    , preparing = 2
    //executing the action
    , executing = 3
    //errors controlled by the code
    , error = 4
    //during the stop period before "finished"
    , stoppingByUser = 5
    //if when "finished" previously was stoppingByUser
    , stoppedByUser = 6
    //if the action has a timeout and hits it before succeeding
    , timeOut = 7
    //when it finishes without issues
    , success = 8
    //during the killing period before "finished"
    , killingByUser = 9
    //killed by the user, not all the actions can be killed, only "special" ones like runProcess (only this one right now)
    , killedByUser = 10
};

#endif // ACTONQTSO_ACTIONEXECUTIONSTATES_HPP
