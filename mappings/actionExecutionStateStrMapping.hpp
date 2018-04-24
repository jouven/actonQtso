#ifndef ACTONQTG_ACTIONEXECUTIONSTATESTRMAPPING_H_
#define ACTONQTG_ACTIONEXECUTIONSTATESTRMAPPING_H_

#include "actionExecutionStates.hpp"

#include <QString>
#include <QHash>

#include <unordered_map>

extern const QHash<QString, actionExecutionState_ec> strToActionExecutionStateUMap_glo_sta_con;
extern const std::unordered_map<actionExecutionState_ec, QString> actionExecutionStateToStrUMap_glo_sta_con;

#endif // ACTONQTG_ACTIONEXECUTIONSTATESTRMAPPING_H_
