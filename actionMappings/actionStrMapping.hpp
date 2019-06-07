#ifndef ACTONQTSO_ACTIONSSTRMAPPING_H_
#define ACTONQTSO_ACTIONSSTRMAPPING_H_

#include "actions.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QMap>

#include <unordered_map>

//keys are lower case, remember to use tolower when comparing
extern EXPIMP_ACTONQTSO const QMap<QString, actionType_ec> strToActionTypeMap_ext_con;
//values are camelcase
extern EXPIMP_ACTONQTSO const std::unordered_map<actionType_ec, QString> actionTypeToStrUMap_ext_con;

#endif // ACTONQTSO_ACTIONSSTRMAPPING_H_
