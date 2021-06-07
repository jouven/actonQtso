#ifndef ACTONQTSO_CHECKEXECUTIONSTATES_HPP
#define ACTONQTSO_CHECKEXECUTIONSTATES_HPP

//shared by all types of checks
enum class checkExecutionState_ec
{
    empty
    //check is not going/being/was executed
    , initial
    //when the check is going to be executed
    //(this is only visible when the checks execute sequentially, one check is making the others wait,
    //or when waiting for an available thread, otherwise it will "instantly" go from initial to executing)
    , preparing
    //executing the check
    , executing
    //errors controlled by the code
    , error
    //during the stop period before "finished"
    , stopping
    //if when "finished" previously was stopping
    , stopped
    //if the check has a timeout and hits it before anything (to be implemented)
    , timeOut
    //when it finishes without issues
    , success
};

#endif // ACTONQTSO_CHECKEXECUTIONSTATES_HPP
