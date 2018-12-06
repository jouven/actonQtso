#include "checkStrMapping.hpp"

const QMap<QString, checkType_ec> strToCheckTypeMap_glo_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"actionfinished", checkType_ec::actionFinished}
    , {	"samefile", checkType_ec::sameFile}

});

const std::unordered_map<checkType_ec, QString> checkTypeToStrUMap_glo_sta_con(
{
    {	checkType_ec::actionFinished, "actionFinished" }
    , {	checkType_ec::sameFile, "sameFile" }

});
