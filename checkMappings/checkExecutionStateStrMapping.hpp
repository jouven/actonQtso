#ifndef ACTONQTSO_CHECKEXECUTIONSTATESTRMAPPING_H_
#define ACTONQTSO_CHECKEXECUTIONSTATESTRMAPPING_H_

#include "checkExecutionStates.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QHash>

#include <unordered_map>


//extern const QHash<QString, checkExecutionState_ec> strToCheckExecutionStateUMap_glo_sta_con;
//only output for now
extern EXPIMP_ACTONQTSO const std::unordered_map<checkExecutionState_ec, QString> checkExecutionStateToStrUMap_ext_con;

#endif // ACTONQTSO_CHECKEXECUTIONSTATESTRMAPPING_H_
