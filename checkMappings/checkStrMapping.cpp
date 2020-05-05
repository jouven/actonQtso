#include "checkStrMapping.hpp"

const QMap<QString, checkType_ec> strToCheckTypeMap_ext_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"actionfinished", checkType_ec::actionFinished}
    , {	"samefile", checkType_ec::sameFile}
    , {	"pathexists", checkType_ec::pathExists}
    , {	"timer", checkType_ec::timer}
    , {	"actionstartedexecuting", checkType_ec::actionStartedExecuting}
});

const std::unordered_map<checkType_ec, QString> checkTypeToStrUMap_ext_con(
{
    {	checkType_ec::actionFinished, "actionFinished" }
    , {	checkType_ec::sameFile, "sameFile" }
    , {	checkType_ec::pathExists, "pathExists" }
    , {	checkType_ec::timer, "timer" }
    , {	checkType_ec::actionStartedExecuting, "actionStartedExecuting" }
});
