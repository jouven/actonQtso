#include "checkMapping.hpp"

#include "../checkData.hpp"

#include "../checks/actionFinished.hpp"
#include "../checks/sameFile.hpp"
#include "../checks/pathExists.hpp"
#include "../checks/timer.hpp"

const std::unordered_map<checkType_ec, std::function<check_c*()>> checkTypeToCheckCreationFunctionMap_ext_con(
{
    {	checkType_ec::sameFile, []() -> check_c* { return new sameFileCheck_c;}}
    , {	checkType_ec::actionFinished, []() -> check_c* { return new actionFinishedCheck_c;}}
    , {	checkType_ec::pathExists, []() -> check_c* { return new pathExistsCheck_c;}}
    , {	checkType_ec::timer, []() -> check_c* { return new timerCheck_c;}}
});
