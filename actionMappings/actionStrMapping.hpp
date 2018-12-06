#ifndef ACTONQTSO_ACTIONSSTRMAPPING_H_
#define ACTONQTSO_ACTIONSSTRMAPPING_H_

#include "actions.hpp"

#include <QString>
#include <QMap>

#include <unordered_map>

//keys are lower case
extern const QMap<QString, actionType_ec> strToActionTypeMap_glo_sta_con;
//values are camelcase
extern const std::unordered_map<actionType_ec, QString> actionTypeToStrUMap_glo_sta_con;

#endif // ACTONQTSO_ACTIONSSTRMAPPING_H_
