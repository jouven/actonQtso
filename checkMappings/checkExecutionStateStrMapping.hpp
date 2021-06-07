#ifndef ACTONQTSO_CHECKEXECUTIONSTATESTRMAPPING_H_
#define ACTONQTSO_CHECKEXECUTIONSTATESTRMAPPING_H_

#include "checkExecutionStates.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QList>

extern EXPIMP_ACTONQTSO QString checkExecutionStateToString_f(const checkExecutionState_ec checkExecutionState_par_con);
extern EXPIMP_ACTONQTSO checkExecutionState_ec stringToCheckExecutionState_f(const QString& string_par_con);
extern EXPIMP_ACTONQTSO QList<QString> checkEecutionStateStringValues_f();

#endif // ACTONQTSO_CHECKEXECUTIONSTATESTRMAPPING_H_
