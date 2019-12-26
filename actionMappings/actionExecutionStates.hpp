#ifndef ACTONQTSO_ACTIONEXECUTIONSTATES_HPP
#define ACTONQTSO_ACTIONEXECUTIONSTATES_HPP

//shared by all types of action executions
enum class actionExecutionState_ec
{
    //action is not going/being/was executed
    initial = 0
    //executing checks
    , executingChecks = 1
    //when the action is going to be executed (this is only visible waiting for an available thread, otherwise it will "instantly" go from initial/executingChecks to executing)
    , preparing = 2
    //executing the action
    , executing = 3
    //errors controlled by the code
    , error = 4
    //during the stop period before "finished"
    , stopping = 5
    //if when "finished" previously was stopping
    , stopped = 6
    //if the action has a timeout and hits it before succeeding
    , timeOut = 7
    //when it finishes without issues
    , success = 8
    //during the killing period before "finished"
    , killing = 9
    //killed by the user, not all the actions can be killed, only "special" ones like runProcess (only this one right now)
    , killed = 10
    //finishing on checks failed, after executingChecks, when the action checks result/s in a false "logic result" and the action won't execute
    , checksFailed = 11
};

#endif // ACTONQTSO_ACTIONEXECUTIONSTATES_HPP
