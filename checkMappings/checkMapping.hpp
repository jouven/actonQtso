#ifndef ACTONQTSO_CHECKSMAPPING_H_
#define ACTONQTSO_CHECKSMAPPING_H_

#include "checks.hpp"

#include "../crossPlatformMacros.hpp"

#include <unordered_map>
#include <functional>

class check_c;

extern EXPIMP_ACTONQTSO const std::unordered_map<checkType_ec, std::function<check_c*()>> checkTypeToCheckCreationFunctionMap_ext_con;

#endif // ACTONQTSO_CHECKSMAPPING_H_
