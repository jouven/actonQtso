#ifndef ACTONQTSO_ACTIONSMAPPING_H_
#define ACTONQTSO_ACTIONSMAPPING_H_

#include "actions.hpp"

#include "../crossPlatformMacros.hpp"

class action_c;
//class actonDataHub_c;

//extern EXPIMP_ACTONQTSO const std::unordered_map<actionType_ec, std::function<action_c*()>> actionTypeToActionCreationFunctionMap_ext_con;
extern EXPIMP_ACTONQTSO action_c* createNewActionOfType_f(const actionType_ec type_par_con);

#endif // ACTONQTSO_ACTIONSSTRMAPPING_H_
