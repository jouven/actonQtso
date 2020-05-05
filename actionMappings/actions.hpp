#ifndef ACTONQTSO_ACTIONS_HPP
#define ACTONQTSO_ACTIONS_HPP

//"TODO"-IMPORTANT
//Why enum empty value?
//when deserializing, it's easy to know if there isn't a field, but it's hard to know when the field is there
//if the value of the field is valid without comparing to all the valid values...
//Since a map/hash is used to map the valid values, Qt QMap has a default case when none matches any of the keys
//SO... add an enum "empty = 0" value to use as default this way it's "easy" to know when a type is there but doesn't match any of the valid.
//This works because default ctored enums are zero-initialized, see https://stackoverflow.com/questions/53897991/enum-class-default-initialization
//What to do with a non valid value when loading? if possible discard the whole object using that non-valid "type" field,
//else use default value for that "type" field from a default ctored object

enum class actionType_ec
{
    empty = 0
    , runProcess = 1
    , createDirectory = 2
    //FUTURE
    , changeWorkingDirectory = 3
    , deleteFileDir = 4
    //FUTURE
    , closeActon = 5
    //FUTURE
    , findFilesDirectories = 6
    //FUTURE
    , modifyEnvironment = 7
    //move too
    , copyFile = 8
    //this is a meta action to stop-end the execution cycle
    //it ends actions with keepExecuting_pro = true
    //two settings:
    //"stop": regular stop
    //"wait to finish": already executing actions/checks will keep executing till they finish but no new check/action will execute (keepExecuting_pro = true included)
    , metaEndExecutionCycle = 9
    //action that monitors a  directory and if a "change criteria" is met it does something
    //i.e. checks for new files and when a new file is found it moves/copies/calls a program with the new file as the first argument...
    , folderChangeReaction = 10
    //if anything modify folderChangeReaction to work on a single file (add a check that the path is a file and if it's not error)
//    //action that monitors a file and if a "change criteria" is met it does something
//    //i.e. if the date/size/hash changes move/copie/call a program with the file as the first argument...
//    , fileChangeReaction = 11
};

#endif // ACTONQTSO_ACTIONS_HPP

