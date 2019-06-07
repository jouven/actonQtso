#ifndef ACTONQTSO_ACTIONSMAPPING_H_
#define ACTONQTSO_ACTIONSMAPPING_H_

#include "actions.hpp"

#include "../crossPlatformMacros.hpp"

#include <unordered_map>
#include <functional>

class action_c;

extern EXPIMP_ACTONQTSO const std::unordered_map<actionType_ec, std::function<action_c*()>> actionTypeToActionCreationFunctionMap_ext_con;

#endif // ACTONQTSO_ACTIONSSTRMAPPING_H_
