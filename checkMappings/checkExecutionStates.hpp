#ifndef ACTONQTSO_CHECKEXECUTIONSTATES_HPP
#define ACTONQTSO_CHECKEXECUTIONSTATES_HPP

//shared by all types of checks
enum class checkExecutionState_ec
{
    //default state on check execution object creation
    initial = 0
    //when the qThread for the check starts but before the check really starts
    , preparing = 1
    //executing the check
    , executing = 2
    //errors controlled by the code
    , error = 3
    //during the stop period before "finished"
    , stoppingByUser = 4
    //if when "finished" previously was stoppingByUser
    , stoppedByUser = 5
    //if the check has a timeout and hits it before anything (to be implemented)
    , timeOut = 6
    //when it finishes without issues
    , success = 7
};

#endif // ACTONQTSO_CHECKEXECUTIONSTATES_HPP
