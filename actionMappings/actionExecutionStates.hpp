#ifndef ACTONQTSO_ACTIONEXECUTIONSTATES_HPP
#define ACTONQTSO_ACTIONEXECUTIONSTATES_HPP

//shared by all types of action executions
enum class actionExecutionState_ec
{
    empty
    //action is not going/being/was executed
    , initial
    //executing checks
    , executingChecks
    //when the action is going to be executed (this is only visible waiting for an available thread, otherwise it will "instantly" go from initial/executingChecks to executing)
    , preparing
    //executing the action
    , executing
    //errors controlled by the code
    , error
    //during the stop period before "finished"
    , stopping
    //if when "finished" previously was stopping
    , stopped
    //if the action has a timeout and hits it before succeeding
    , timeOut
    //when it finishes without issues
    , success
    //during the killing period before "finished"
    , killing
    //killed by the user, not all the actions can be killed, only "special" ones like runProcess (only this one right now)
    , killed
    //finishing on checks failed, after executingChecks, when the action checks result/s in a false "logic result" and the action won't execute
    , checksFailed
};

#endif // ACTONQTSO_ACTIONEXECUTIONSTATES_HPP
