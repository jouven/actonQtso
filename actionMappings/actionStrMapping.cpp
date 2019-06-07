#include "actionStrMapping.hpp"

const QMap<QString, actionType_ec> strToActionTypeMap_ext_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"runprocess", actionType_ec::runProcess}
    , {	"createdirectory", actionType_ec::createDirectory}
    , {	"copyfile", actionType_ec::copyFile}

});

const std::unordered_map<actionType_ec, QString> actionTypeToStrUMap_ext_con(
{
//these ones can be in camelcase or w/e
    {	actionType_ec::runProcess, "runProcess" }
    , {	actionType_ec::createDirectory, "createDirectory" }
    , {	actionType_ec::changeWorkingDirectory, "changeWorkingDirectory" }
    , {	actionType_ec::deleteFileDir, "deleteFileDir" }
    , {	actionType_ec::closeActon, "closeActon" }
    , {	actionType_ec::findFilesDirectories, "findFilesDirectories" }
    , {	actionType_ec::modifyEnvironment, "modifyEnvironment" }
    , {	actionType_ec::copyFile, "copyFile" }
});

