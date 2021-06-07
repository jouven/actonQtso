#ifndef ACTONQTSO_ACTIONEXECUTIONSTATESTRMAPPING_H_
#define ACTONQTSO_ACTIONEXECUTIONSTATESTRMAPPING_H_

#include "actionExecutionStates.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QList>

extern EXPIMP_ACTONQTSO QString actionExecutionStateToString_f(const actionExecutionState_ec actionExecutionState_par_con);
extern EXPIMP_ACTONQTSO actionExecutionState_ec stringToActionExecutionState_f(const QString& string_par_con);
extern EXPIMP_ACTONQTSO QList<QString> actionExecutionStateStringValues_f();

#endif // ACTONQTSO_ACTIONEXECUTIONSTATESTRMAPPING_H_
