#ifndef ACTONQTG_ACTIONSSTRMAPPING_H_
#define ACTONQTG_ACTIONSSTRMAPPING_H_

#include "actions.hpp"

#include <QString>
#include <QHash>

#include <unordered_map>

extern const QHash<QString, action_ec> strToActionUMap_glo_sta_con;
extern const std::unordered_map<action_ec, QString> actionToStrUMap_glo_sta_con;

#endif // ACTONQTG_ACTIONSSTRMAPPING_H_
