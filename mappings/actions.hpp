#ifndef ACTONQTG_ACTIONS_HPP
#define ACTONQTG_ACTIONS_HPP

//#include <cstdint>

enum class action_ec
{
    runProcess = 0
    , createDirectory = 1
    //TODO bellow
    , changeWorkingDirectory = 2
    , deleteX = 3 //can't use delete alone (protected keyword)
    , closeActon = 4
    , findFilesDirectories = 5
    , modifyEnvironment = 6
};

#endif // ACTONQTG_ACTIONS_HPP
