#ifndef ACTONQTSO_ACTIONS_HPP
#define ACTONQTSO_ACTIONS_HPP

//#include <cstdint>

enum class actionType_ec
{
    empty = 0
    , runProcess = 1
    , createDirectory = 2
    //FUTURE bellow
    , changeWorkingDirectory = 3
    , deleteFileDir = 4
    , closeActon = 5
    , findFilesDirectories = 6
    , modifyEnvironment = 7
};

#endif // ACTONQTSO_ACTIONS_HPP

