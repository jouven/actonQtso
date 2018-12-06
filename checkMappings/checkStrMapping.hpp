#ifndef ACTONQTSO_CHECKSSTRMAPPING_H_
#define ACTONQTSO_CHECKSSTRMAPPING_H_

#include "checks.hpp"

#include <QString>
#include <QMap>

#include <unordered_map>

//keys are lower case
extern const QMap<QString, checkType_ec> strToCheckTypeMap_glo_sta_con;
//values are camelcase
extern const std::unordered_map<checkType_ec, QString> checkTypeToStrUMap_glo_sta_con;

#endif // ACTONQTSO_CHECKSSTRMAPPING_H_
