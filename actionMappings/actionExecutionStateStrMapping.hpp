#ifndef ACTONQTSO_ACTIONEXECUTIONSTATESTRMAPPING_H_
#define ACTONQTSO_ACTIONEXECUTIONSTATESTRMAPPING_H_

#include "actionExecutionStates.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
//#include <QHash>

#include <unordered_map>


//extern const QHash<QString, actionExecutionState_ec> strToActionExecutionStateUMap_ext_con;
//only output for now
extern EXPIMP_ACTONQTSO const std::unordered_map<actionExecutionState_ec, QString> actionExecutionStateToStrUMap_ext_con;

#endif // ACTONQTSO_ACTIONEXECUTIONSTATESTRMAPPING_H_
