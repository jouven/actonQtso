#include "actionStrMapping.hpp"

const QHash<QString, action_ec> strToActionUMap_glo_sta_con(
{
    {	"RunProcess", action_ec::runProcess}
    , {	"CreateDirectory", action_ec::createDirectory}

});

const std::unordered_map<action_ec, QString> actionToStrUMap_glo_sta_con(
{
    {	action_ec::runProcess, "RunProcess" }
    , {	action_ec::createDirectory, "CreateDirectory" }

});
