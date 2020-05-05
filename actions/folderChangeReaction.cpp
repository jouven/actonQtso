#include "folderChangeReaction.hpp"

#include "../actionExecution/folderChangeReactionExecution.hpp"
#include "../actionMappings/actionStrMapping.hpp"
#include "../actonDataHub.hpp"
#include "../reused/stringAlgo.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"
#include "comuso/practicalTemplates.hpp"

#include <QJsonObject>
#include <QJsonArray>

//changeType_ec mapping
const QMap<QString, folderChangeReactionData_c::changeType_ec> folderChangeReactionData_c::strToChangeTypeMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"filecreated", folderChangeReactionData_c::changeType_ec::fileCreated}
    , {	"fileremoved", folderChangeReactionData_c::changeType_ec::fileRemoved}
    , {	"filesizechanged", folderChangeReactionData_c::changeType_ec::fileSizeChanged}
    , {	"filemodifieddatetimechanged", folderChangeReactionData_c::changeType_ec::fileModifiedDateTimeChanged}
    , {	"filecontentschanged", folderChangeReactionData_c::changeType_ec::fileContentsChanged}
});

const std::unordered_map<folderChangeReactionData_c::changeType_ec, QString> folderChangeReactionData_c::changeTypeToStrUMap_sta_con(
{
    {	folderChangeReactionData_c::changeType_ec::fileCreated, "fileCreated" }
    , {	folderChangeReactionData_c::changeType_ec::fileRemoved, "fileRemoved" }
    , {	folderChangeReactionData_c::changeType_ec::fileSizeChanged, "fileSizeChanged" }
    , {	folderChangeReactionData_c::changeType_ec::fileModifiedDateTimeChanged, "fileModifiedDateTimeChanged" }
    , {	folderChangeReactionData_c::changeType_ec::fileContentsChanged, "fileContentsChanged" }
});

//fileSizeChangeType_ec mapping
const QMap<QString, folderChangeReactionData_c::fileSizeChangeType_ec> folderChangeReactionData_c::strToFileSizeChangeTypeMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"any", folderChangeReactionData_c::fileSizeChangeType_ec::any}
    , {	"threshold", folderChangeReactionData_c::fileSizeChangeType_ec::threshold}
    , {	"fixedvalue", folderChangeReactionData_c::fileSizeChangeType_ec::fixedValue}
    , {	"anydecrease", folderChangeReactionData_c::fileSizeChangeType_ec::anyDecrease}
    , {	"anyincrease", folderChangeReactionData_c::fileSizeChangeType_ec::anyIncrease}
});

const std::unordered_map<folderChangeReactionData_c::fileSizeChangeType_ec, QString> folderChangeReactionData_c::fileSizeChangeTypeToStrUMap_sta_con(
{
    {	folderChangeReactionData_c::fileSizeChangeType_ec::any, "any" }
    , {	folderChangeReactionData_c::fileSizeChangeType_ec::threshold, "threshold" }
    , {	folderChangeReactionData_c::fileSizeChangeType_ec::fixedValue, "fixedValue" }
    , {	folderChangeReactionData_c::fileSizeChangeType_ec::anyDecrease, "anyDecrease" }
    , {	folderChangeReactionData_c::fileSizeChangeType_ec::anyIncrease, "anyIncrease" }
});


//reactionType_ec mapping
const QMap<QString, folderChangeReactionData_c::reactionType_ec> folderChangeReactionData_c::strToReactionTypeMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"copymove", folderChangeReactionData_c::reactionType_ec::copyMove}
    , {	"remove", folderChangeReactionData_c::reactionType_ec::remove}
    , {	"execute", folderChangeReactionData_c::reactionType_ec::execute}
    , {	"runprocess", folderChangeReactionData_c::reactionType_ec::runProcess}
});

const std::unordered_map<folderChangeReactionData_c::reactionType_ec, QString> folderChangeReactionData_c::reactionTypeToStrUMap_sta_con(
{
    {	folderChangeReactionData_c::reactionType_ec::copyMove, "copyMove" }
    , {	folderChangeReactionData_c::reactionType_ec::remove, "remove" }
    , {	folderChangeReactionData_c::reactionType_ec::execute, "execute" }
    , {	folderChangeReactionData_c::reactionType_ec::runProcess, "runProcess" }
});

//reactionOrder_ec mapping
const QMap<QString, folderChangeReactionData_c::reactionOrder_ec> folderChangeReactionData_c::strToReactionOrderMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"newestfirst", folderChangeReactionData_c::reactionOrder_ec::newestFirst}
    , {	"alphabetical", folderChangeReactionData_c::reactionOrder_ec::alphabetical}
    , {	"smallerfirst", folderChangeReactionData_c::reactionOrder_ec::smallerFirst}
});

const std::unordered_map<folderChangeReactionData_c::reactionOrder_ec, QString> folderChangeReactionData_c::reactionOrderToStrUMap_sta_con(
{
    {	folderChangeReactionData_c::reactionOrder_ec::newestFirst, "newestFirst" }
    , {	folderChangeReactionData_c::reactionOrder_ec::alphabetical, "alphabetical" }
    , {	folderChangeReactionData_c::reactionOrder_ec::smallerFirst, "smallerFirst" }
});

bool folderChangeReactionData_c::recurseFolders_f() const
{
    return recurseFolders_pro;
}

void folderChangeReactionData_c::setRecurseFolders_f(const bool recurseFolders_par_con)
{
    recurseFolders_pro = recurseFolders_par_con;
}

bool folderChangeReactionData_c::useAbsolutePaths_f() const
{
    return useAbsolutePaths_pro;
}

void folderChangeReactionData_c::setUseAbsolutePaths_f(const bool useAbsolutePath_par_con)
{
    useAbsolutePaths_pro = useAbsolutePath_par_con;
}

QString folderChangeReactionData_c::argumentPlaceHolderToReplaceWithChangedFilePathParsed_f() const
{
    COPYPARSERETURNVAR(argumentPlaceHolderToReplaceWithChangedFilePath_pro)
}

QString folderChangeReactionData_c::argumentPlaceHolderToReplaceWithChangedFilePath_f() const
{
    return argumentPlaceHolderToReplaceWithChangedFilePath_pro;
}

void folderChangeReactionData_c::setArgumentPlaceHolderToReplaceWithChangedFilePath_f(const QString& argumentPlaceHolderToReplaceWithChangedFilePath_par_con)
{
    argumentPlaceHolderToReplaceWithChangedFilePath_pro = argumentPlaceHolderToReplaceWithChangedFilePath_par_con;
}

QString folderChangeReactionData_c::reactionActionStringId_f() const
{
    return reactionActionStringId_pro;
}

//QString folderChangeReactionData_c::reactionActionStringIdParsed_f() const
//{
//    COPYPARSERETURNVAR(reactionActionStringId_pro)
//}

void folderChangeReactionData_c::setReactionActionStringId_f(const QString& reactionActionStringId_par_con)
{
    reactionActionStringId_pro = reactionActionStringId_par_con;
}

bool folderChangeReactionData_c::reverseReactionOrder_f() const
{
    return reverseReactionOrder_pro;
}

void folderChangeReactionData_c::setReverseReactionOrder_f(const bool reverseReactionOrder_par_con)
{
    reverseReactionOrder_pro = reverseReactionOrder_par_con;
}

bool folderChangeReactionData_c::isValidActionForReaction_f(
        const action_c* const action_par
        , const reactionType_ec reactionType_par_con
)
{
    bool resultTmp(false);
    while (action_par not_eq nullptr)
    {
        if (action_par->enabled_f())
        {
            break;
        }
        if (equalOnce_ft(reactionType_par_con, folderChangeReactionData_c::reactionType_ec::execute, folderChangeReactionData_c::reactionType_ec::runProcess))
        {
            if (action_par->type_f() == actionType_ec::runProcess)
            {
                resultTmp = true;
            }
            break;
        }

        if (reactionType_par_con == folderChangeReactionData_c::reactionType_ec::remove)
        {
            if (action_par->type_f() == actionType_ec::deleteFileDir)
            {
                resultTmp = true;
            }
            break;
        }

        if (reactionType_par_con == folderChangeReactionData_c::reactionType_ec::copyMove)
        {
            if (action_par->type_f() == actionType_ec::copyFile)
            {
                resultTmp = true;
            }
            break;
        }
        break;
    }
    return resultTmp;
}

folderChangeReactionData_c::folderChangeReactionData_c(
        const QString& folderPath_par_con
        , const bool recurseFolders_par_con
        , const bool useAbsolutePaths_par_con
        , const std::unordered_set<changeType_ec>& changesToMonitor_par_con
        , const folderChangeReactionData_c::fileSizeChangeType_ec fileSizeChangeTypeToMonitor_par_con
        , const uint_fast64_t fileSizeChangeFixedValue_par_con
        , const int_fast64_t fileSizeChangeThreshold_par_con
        , const folderChangeReactionData_c::reactionType_ec reactionType_par_con
        , const QString& argumentPlaceHolderToReplaceWithChangedFilePath_par_con
        , const bool sequentialReaction_par_con
        , const folderChangeReactionData_c::reactionOrder_ec reactionOrder_par_con
        , const bool reverseReactionOrder_par_con
        , const uint_fast64_t monitorIntervalMs_par_con
        , const QString& reactionActionStringId_par_con)
    : folderPath_pro(folderPath_par_con)
    , recurseFolders_pro(recurseFolders_par_con)
    , useAbsolutePaths_pro(useAbsolutePaths_par_con)
    , changesToMonitor_pro(changesToMonitor_par_con)
    , fileSizeChangeTypeToMonitor_pro(fileSizeChangeTypeToMonitor_par_con)
    , fileSizeChangeFixedValue_pro(fileSizeChangeFixedValue_par_con)
    , fileSizeChangeThreshold_pro(fileSizeChangeThreshold_par_con)
    , reactionType_pro(reactionType_par_con)
    , argumentPlaceHolderToReplaceWithChangedFilePath_pro(argumentPlaceHolderToReplaceWithChangedFilePath_par_con)
    , sequentialReaction_pro(sequentialReaction_par_con)
    , reactionOrder_pro(reactionOrder_par_con)
    , reverseReactionOrder_pro(reverseReactionOrder_par_con)
    , monitorIntervalMs_pro(monitorIntervalMs_par_con)
    , reactionActionStringId_pro(reactionActionStringId_par_con)
{}

bool folderChangeReactionData_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
        text_c errorTextTmp;
        if (isValidStringSize_f(folderPathParsed_f(), 255, std::addressof(errorTextTmp), "Folder path is too long: {0} (maximum length is {1})"))
        {
            //it's valid
        }
        else
        {
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
            break;
        }
        if (folderPathParsed_f().isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Folder path is empty")
            break;
        }

        if (changesToMonitor_pro.empty())
        {
            APPENDTEXTPTR(errorsPtr_par, "No changes to monitor")
            break;
        }

        bool monitorsFileSizeChangesTmp(changesToMonitor_pro.count(changeType_ec::fileSizeChanged) > 0);
        if (monitorsFileSizeChangesTmp and fileSizeChangeTypeToMonitor_pro == fileSizeChangeType_ec::empty)
        {
            APPENDTEXTPTR(errorsPtr_par, "No file size change type to monitor");
            break;
        }
        if (reactionType_pro == reactionType_ec::empty)
        {
            APPENDTEXTPTR(errorsPtr_par, "Empty reaction type");
            break;
        }
        //TODO a reaction action will always need an additional action
        //to execute-use for the reaction part
        //for now don't validate if the actionStringId points to an actual disabled action here
        //do it on execution, because anyway this can be done also on any program/X that links to this library
        //while editing the action or somewhere else
        if (reactionActionStringId_pro.isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Empty reaction Action stringId type");
            break;
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}

QString folderChangeReactionData_c::folderPath_f() const
{
    return folderPath_pro;
}

QString folderChangeReactionData_c::folderPathParsed_f() const
{
    COPYPARSERETURNVAR(folderPath_pro)
}

void folderChangeReactionData_c::setFolderPath_f(const QString& folderPath_par_con)
{
    folderPath_pro = folderPath_par_con;
}

std::unordered_set<folderChangeReactionData_c::changeType_ec> folderChangeReactionData_c::changesToMonitor_f() const
{
    return changesToMonitor_pro;
}

void folderChangeReactionData_c::setChangesToMonitor_pri(const std::unordered_set<changeType_ec>& changesToMonitor_par_con)
{
    changesToMonitor_pro = changesToMonitor_par_con;
}

folderChangeReactionData_c::fileSizeChangeType_ec folderChangeReactionData_c::fileSizeChangeTypeToMonitor_f() const
{
    return fileSizeChangeTypeToMonitor_pro;
}

void folderChangeReactionData_c::setFileSizeChangeTypeToMonitor_f(const fileSizeChangeType_ec& fileSizeChangeTypeToMonitor_par_con)
{
    fileSizeChangeTypeToMonitor_pro = fileSizeChangeTypeToMonitor_par_con;
}

int_fast64_t folderChangeReactionData_c::fileSizeChangeFixedValue_f() const
{
    return fileSizeChangeFixedValue_pro;
}

void folderChangeReactionData_c::setFileSizeChangeFixedValue_f(const int_fast64_t& fileSizeChangeFixedValue_par_con)
{
    fileSizeChangeFixedValue_pro = fileSizeChangeFixedValue_par_con;
}

int_fast64_t folderChangeReactionData_c::fileSizeChangeThreshold_f() const
{
    return fileSizeChangeThreshold_pro;
}

void folderChangeReactionData_c::setFileSizeChangeThreshold_pri(const int_fast64_t& fileSizeChangeThreshold_par_con)
{
    fileSizeChangeThreshold_pro = fileSizeChangeThreshold_par_con;
}

folderChangeReactionData_c::reactionType_ec folderChangeReactionData_c::reactionType_f() const
{
    return reactionType_pro;
}

void folderChangeReactionData_c::setReactionType_pri(const reactionType_ec& reactionType_par_con)
{
    reactionType_pro = reactionType_par_con;
}

bool folderChangeReactionData_c::sequentialReaction_f() const
{
    return sequentialReaction_pro;
}

void folderChangeReactionData_c::setSequentialReaction_pri(const bool sequentialReaction_par_con)
{
    sequentialReaction_pro = sequentialReaction_par_con;
}

folderChangeReactionData_c::reactionOrder_ec folderChangeReactionData_c::reactionOrder_f() const
{
    return reactionOrder_pro;
}

void folderChangeReactionData_c::setReactionOrder_pri(const reactionOrder_ec& reactionOrder_par_con)
{
    reactionOrder_pro = reactionOrder_par_con;
}

uint_fast64_t folderChangeReactionData_c::monitorIntervalMs_f() const
{
    return monitorIntervalMs_pro;
}

void folderChangeReactionData_c::setMonitorIntervalMs_f(const uint_fast64_t& monitorIntervalMs_par_con)
{
    monitorIntervalMs_pro = monitorIntervalMs_par_con;
}


void folderChangeReactionAction_c::derivedWrite_f(QJsonObject& json_par) const
{
    QJsonArray changesToMonitorArrayTmp;
    for (const auto changeToMonitor_ite_con : changesToMonitor_pro)
    {
        changesToMonitorArrayTmp.append(QJsonValue(changeTypeToStrUMap_sta_con.at(changeToMonitor_ite_con)));
    }
    if (changesToMonitorArrayTmp.empty())
    {
        text_c errorTmp("folderChangeReaction action has no changes to monitor", reactionActionStringId_pro);
        MACRO_ADDACTONQTSOLOG(errorTmp, this, logItem_c::type_ec::error);
    }
    else
    {
        json_par["changesToMonitor"] = changesToMonitorArrayTmp;
    }
    json_par["folderPath"] = folderPath_pro;
    json_par["recurseFolders"] = recurseFolders_pro;
    json_par["useAbsolutePaths"] = useAbsolutePaths_pro;
    if (fileSizeChangeTypeToMonitor_pro not_eq fileSizeChangeType_ec::empty)
    {
        json_par["fileSizeChangeTypeToMonitor"] = fileSizeChangeTypeToStrUMap_sta_con.at(fileSizeChangeTypeToMonitor_pro);
    }
    if (fileSizeChangeTypeToMonitor_pro == fileSizeChangeType_ec::fixedValue)
    {
        json_par["fileSizeChangeFixedValue"] = QString::number(fileSizeChangeFixedValue_pro);
    }
    if (fileSizeChangeTypeToMonitor_pro == fileSizeChangeType_ec::threshold)
    {
        json_par["fileSizeChangeThreshold"] = QString::number(fileSizeChangeThreshold_pro);
    }
    if (reactionType_pro not_eq reactionType_ec::empty)
    {
        json_par["reactionType"] = reactionTypeToStrUMap_sta_con.at(reactionType_pro);
        if (reactionType_pro == reactionType_ec::runProcess)
        {
            json_par["argumentPlaceHolderToReplaceWithChangedFilePath"] = argumentPlaceHolderToReplaceWithChangedFilePath_pro;
        }
    }
    json_par["sequentialReaction"] = sequentialReaction_pro;
    if (reactionOrder_pro not_eq reactionOrder_ec::empty)
    {
        json_par["reactionOrder"] = reactionOrderToStrUMap_sta_con.at(reactionOrder_pro);
        json_par["reverseReactionOrder"] = reverseReactionOrder_pro;
    }
    json_par["monitorIntervalMs"] = QString::number(monitorIntervalMs_pro);
    json_par["reactionActionStringId"] = reactionActionStringId_pro;
}

void folderChangeReactionAction_c::derivedRead_f(const QJsonObject& json_par_con)
{
    if (not json_par_con["changesToMonitor"].isUndefined())
    {
        QJsonArray jsonArrayChangesToMonitorTmp(json_par_con["changesToMonitor"].toArray());
        if (not jsonArrayChangesToMonitorTmp.isEmpty())
        {
            changesToMonitor_pro.reserve(jsonArrayChangesToMonitorTmp.size());
            for (const auto jsonArrayItem_ite_con : jsonArrayChangesToMonitorTmp)
            {
                changesToMonitor_pro.emplace(strToChangeTypeMap_sta_con.value(jsonArrayItem_ite_con.toString().toLower()));
            }
        }
    }
    folderPath_pro = json_par_con["folderPath"].toString();
    if (json_par_con["recurseFolders"].isBool())
    {
        recurseFolders_pro = json_par_con["recurseFolders"].toBool();
    }
    if (json_par_con["useAbsolutePaths"].isBool())
    {
        useAbsolutePaths_pro = json_par_con["useAbsolutePaths"].toBool();
    }
    if (json_par_con["fileSizeChangeTypeToMonitor"].isString())
    {
        fileSizeChangeTypeToMonitor_pro = strToFileSizeChangeTypeMap_sta_con.value(json_par_con["fileSizeChangeTypeToMonitor"].toString().toLower());
        if (fileSizeChangeTypeToMonitor_pro == fileSizeChangeType_ec::fixedValue and json_par_con["fileSizeChangeFixedValue"].isString())
        {
            bool goodConversionTmp(false);
            uint_fast64_t conversionResultTmp(json_par_con["fileSizeChangeFixedValue"].toString().toULongLong(std::addressof(goodConversionTmp)));
            if (goodConversionTmp and conversionResultTmp > 0)
            {
                fileSizeChangeFixedValue_pro = conversionResultTmp;
            }
        }
        if (fileSizeChangeTypeToMonitor_pro == fileSizeChangeType_ec::threshold and json_par_con["fileSizeChangeThreshold"].isString())
        {
            bool goodConversionTmp(false);
            int_fast64_t conversionResultTmp(json_par_con["fileSizeChangeThreshold"].toString().toLongLong(std::addressof(goodConversionTmp)));
            if (goodConversionTmp and conversionResultTmp > 0)
            {
                fileSizeChangeThreshold_pro = conversionResultTmp;
            }
        }
    }
    if (json_par_con["reactionType"].isString())
    {
        reactionType_pro = strToReactionTypeMap_sta_con.value(json_par_con["reactionType"].toString().toLower());
        if (reactionType_pro == reactionType_ec::runProcess and json_par_con["argumentPlaceHolderToReplaceWithChangedFilePath"].isString())
        {
            argumentPlaceHolderToReplaceWithChangedFilePath_pro = json_par_con["argumentPlaceHolderToReplaceWithChangedFilePath"].toString();
        }
    }
    if (json_par_con["sequentialReaction"].isBool())
    {
        sequentialReaction_pro = json_par_con["sequentialReaction"].toBool();
    }
    if (json_par_con["reactionOrder"].isString())
    {
        reactionOrder_pro = strToReactionOrderMap_sta_con.value(json_par_con["reactionOrder"].toString().toLower());
        if (json_par_con["reverseReactionOrder"].isBool())
        {
            reverseReactionOrder_pro = json_par_con["reverseReactionOrder"].toBool();
        }
    }
    if (json_par_con["monitorIntervalMs"].isString())
    {
        bool goodConversionTmp(false);
        uint_fast64_t integerMaybeTmp(json_par_con["monitorIntervalMs"].toString().toULongLong(std::addressof(goodConversionTmp)));
        if (goodConversionTmp)
        {
            monitorIntervalMs_pro = integerMaybeTmp;
        }
    }
    if (json_par_con["reactionActionStringId"].isString())
    {
        reactionActionStringId_pro = json_par_con["reactionActionStringId"].toString();
    }
}

bool folderChangeReactionAction_c::derivedIsValidAction_f(textCompilation_c* errors_par) const
{
    return isFieldsDataValid_f(errors_par);
}

uint_fast64_t folderChangeReactionAction_c::derivedUpdateActionStringIdDependencies_f(const QString& oldActionStringId_par_con, const QString& newActionStringId_par_con)
{
    return replaceSubString_f(reactionActionStringId_pro, oldActionStringId_par_con, newActionStringId_par_con);
}

uint_fast64_t folderChangeReactionAction_c::derivedActionStringIdDependencyCount_f(const QString& actionStringId_par_con) const
{
    return vectorQStringCountSubString_f(actionStringId_par_con, {reactionActionStringId_pro});
}

uint_fast64_t folderChangeReactionAction_c::derivedUpdateStringTriggerDependecies_f(const QString& oldStringTrigger_par_con, const QString& newStringTrigger_par_con)
{
    return replaceSubString_f(folderPath_pro, oldStringTrigger_par_con, newStringTrigger_par_con)
            + replaceSubString_f(argumentPlaceHolderToReplaceWithChangedFilePath_pro, oldStringTrigger_par_con, newStringTrigger_par_con)
            + replaceSubString_f(reactionActionStringId_pro, oldStringTrigger_par_con, newStringTrigger_par_con);
}

uint_fast64_t folderChangeReactionAction_c::derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const
{
    return vectorQStringCountSubString_f(stringTrigger_par_con, {folderPath_pro, argumentPlaceHolderToReplaceWithChangedFilePath_pro, reactionActionStringId_pro});
}

QSet<QString> folderChangeReactionAction_c::derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const
{
    QSet<QString> resultTmp;
    for (const QString& searchValue_ite_con : searchValues_par_con)
    {
        if (vectorQStringCountSubString_f(searchValue_ite_con, {folderPath_pro, argumentPlaceHolderToReplaceWithChangedFilePath_pro, reactionActionStringId_pro}, true) > 0)
        {
            resultTmp.insert(searchValue_ite_con);
        }
    }
    return resultTmp;
}

action_c* folderChangeReactionAction_c::derivedClone_f() const
{
    //slice and dice
    folderChangeReactionData_c folderChangeReactionDataTmp(*this);
    actionData_c actionDataTmp(*this);
    return new folderChangeReactionAction_c(actionDataTmp, folderChangeReactionDataTmp);
}

baseActionExecution_c* folderChangeReactionAction_c::createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par)
{
    return new folderChangeReactionActionExecution_c(actionDataExecutionResult_ptr_par, this);
}

actionType_ec folderChangeReactionAction_c::type_f() const
{
    return actionType_ec::folderChangeReaction;
}

//QString sentryFolderAction_c::typeStr_f() const
//{
//    return actionTypeToStrUMap_ext_con.at(type_f());
//}

folderChangeReactionAction_c::folderChangeReactionAction_c(
        const actionData_c& actionData_par_con
        , const folderChangeReactionData_c& folderChangeReactionData_par_con)
    : action_c(actionData_par_con)
    , folderChangeReactionData_c(folderChangeReactionData_par_con)
{

}

void folderChangeReactionAction_c::updateFolderChangeReactionData_f(const folderChangeReactionData_c& folderChangeReactionData_par_con)
{
    this->folderChangeReactionData_c::operator=(folderChangeReactionData_par_con);
}
