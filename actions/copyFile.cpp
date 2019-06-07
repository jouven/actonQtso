#include "copyFile.hpp"

#include "../actionExecution/copyFileExecution.hpp"
#include "../actionMappings/actionStrMapping.hpp"
#include "../actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "essentialQtso/macros.hpp"
#include "comuso/practicalTemplates.hpp"

#include <QDir>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutexLocker>

const QMap<QString, copyFileData_c::transferType_ec> copyFileData_c::strToTransferTypeMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"copy", copyFileData_c::transferType_ec::copy}
    , {	"move", copyFileData_c::transferType_ec::move}
    , {	"truemove", copyFileData_c::transferType_ec::trueMove}
});

const std::unordered_map<copyFileData_c::transferType_ec, QString> copyFileData_c::transferTypeToStrUMap_sta_con(
{
    {	copyFileData_c::transferType_ec::copy, "copy" }
    , {	copyFileData_c::transferType_ec::move, "move" }
    , {	copyFileData_c::transferType_ec::trueMove, "trueMove" }

});


const QMap<QString, copyFileData_c::destinationTreatment_ec> copyFileData_c::strToDestinationTreatmentMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"overwrite", copyFileData_c::destinationTreatment_ec::overwrite}
    , {	"nooverwrite", copyFileData_c::destinationTreatment_ec::noOverwrite}
    , {	"trystupidresume", copyFileData_c::destinationTreatment_ec::tryStupidResume}
    , {	"lazyresume", copyFileData_c::destinationTreatment_ec::lazyResume}
    , {	"overwriteiserror", copyFileData_c::destinationTreatment_ec::overwriteIsError}
    , {	"overwriteonlyifdifferent", copyFileData_c::destinationTreatment_ec::overwriteOnlyIfDifferent}
    , {	"overwriteiserroronlyifdifferent", copyFileData_c::destinationTreatment_ec::overwriteIsErrorOnlyIfDifferent}
});

const std::unordered_map<copyFileData_c::destinationTreatment_ec, QString> copyFileData_c::destinationTreatmentToStrUMap_sta_con(
{
    {	copyFileData_c::destinationTreatment_ec::overwrite, "overwrite" }
    , {	copyFileData_c::destinationTreatment_ec::noOverwrite, "noOverwrite" }
    , {	copyFileData_c::destinationTreatment_ec::tryStupidResume, "tryStupidResume" }
    , {	copyFileData_c::destinationTreatment_ec::lazyResume, "lazyResume" }
    , {	copyFileData_c::destinationTreatment_ec::overwriteIsError, "overwriteIsError" }
    , {	copyFileData_c::destinationTreatment_ec::overwriteOnlyIfDifferent, "overwriteOnlyIfDifferent" }
    , {	copyFileData_c::destinationTreatment_ec::overwriteIsErrorOnlyIfDifferent, "overwriteIsErrorOnlyIfDifferent" }
});

QString copyFileData_c::sourcePath_f() const
{
    return sourcePath_pro;
}

QString copyFileData_c::sourcePathParsed_f() const
{
    COPYPARSERETURNVAR(sourcePath_pro);
}

void copyFileData_c::setSourcePath_f(const QString& sourcePath_par_con)
{
    sourcePath_pro = sourcePath_par_con;
}

QString copyFileData_c::destinationPath_f() const
{
    return destinationPath_pro;
}

QString copyFileData_c::destinationPathParsed_f() const
{
    COPYPARSERETURNVAR(destinationPath_pro);
}

void copyFileData_c::setDestinationPath_f(const QString& destinationPath_par_con)
{
    destinationPath_pro = destinationPath_par_con;
}

bool copyFileData_c::navigateSubdirectories_f() const
{
    return navigateSubdirectories_pro;
}

void copyFileData_c::setNavigateSubdirectories_f(const bool includeSubdirectories_par_con)
{
    navigateSubdirectories_pro = includeSubdirectories_par_con;
}

QStringList copyFileData_c::sourceFilenameRegexFilters_f() const
{
    return sourceFilenameRegexFilters_pro;
}

QStringList copyFileData_c::sourceFilenameRegexFiltersParsed_f() const
{
    COPYPARSERETURNSTRINGLIST(sourceFilenameRegexFilters_pro);
}

void copyFileData_c::setSourceFilenameRegexFilters_f(const QStringList& sourceFilenameRegexFilters_par_con)
{
    sourceFilenameRegexFilters_pro = sourceFilenameRegexFilters_par_con;
}

bool copyFileData_c::createDestinationAndParents_f() const
{
    return createDestinationAndParents_pro;
}

void copyFileData_c::setCreateDestinationAndParents_f(const bool createDestinationParent_par_con)
{
    createDestinationAndParents_pro = createDestinationParent_par_con;
}

std::vector<QString> copyFileData_c::testSourceFileList_f(
        QString* error_ptr
        , QMutex* directoryFilterPtrMutexPtr_par)
{
    std::vector<QString> resultTmp;
    while (isValid_f(error_ptr))
    {
        //ignore destination options
        const QString sourcePathTmp_con(sourcePathParsed_f());
        if (QFileInfo::exists(sourcePathTmp_con))
        {
            //good
        }
        else
        {
            if (error_ptr not_eq nullptr)
            {
                error_ptr->append("Source doesn't exist");
            }
            break;
        }

        QFileInfo sourceFileInfoTmp(sourcePathTmp_con);
        if (sourceFileInfoTmp.isDir())
        {
            filterOptions_s filterOptionsTmp;
            filterOptionsTmp.navigateSubdirectories_pub = navigateSubdirectories_pro;
            filterOptionsTmp.useAbsolutePaths_pub = sourceFileInfoTmp.isAbsolute();
            filterOptionsTmp.listFiles_pub = true;
            filterOptionsTmp.listHidden_pub = copyHidden_pro;
            filterOptionsTmp.navigateHiddenDirectories_pub = copyHidden_pro;
            filterOptionsTmp.listDirectories_pub = false;
            filterOptionsTmp.filenameRegexFilters_pub = sourceFilenameRegexFiltersParsed_f();
            filterOptionsTmp.listEmptyDirectories_pub = copyEmptyDirectories_pro;
            filterOptionsTmp.filenameFullExtensions_pub = sourceFilenameFullExtensionsParsed_f();

            directoryFilter_c directoryFilterTmp(sourcePathTmp_con, filterOptionsTmp);
            //the mutex here is to prevent the stop function to conflict
            //with the filtering normal ending process

            if (directoryFilterPtrMutexPtr_par not_eq nullptr)
            {
                {
                    QMutexLocker mutexLockerTmp(directoryFilterPtrMutexPtr_par);
                    directoryFilterPtr_pro = std::addressof(directoryFilterTmp);
                }
                resultTmp = directoryFilterTmp.filter_f();
                if (error_ptr not_eq nullptr and directoryFilterTmp.anyError_f())
                {
                    error_ptr->append(directoryFilterTmp.getError_f());
                }
                {
                    QMutexLocker mutexLockerTmp(directoryFilterPtrMutexPtr_par);
                    directoryFilterPtr_pro = nullptr;
                }
            }
            else
            {
                directoryFilterPtr_pro = std::addressof(directoryFilterTmp);
                resultTmp = directoryFilterTmp.filter_f();
                if (error_ptr not_eq nullptr and directoryFilterTmp.anyError_f())
                {
                    error_ptr->append(directoryFilterTmp.getError_f());
                }
            }
            break;
        }

        if (sourceFileInfoTmp.isFile())
        {
            resultTmp.emplace_back(sourcePathTmp_con);
            break;
        }
        break;
    }
    return resultTmp;
}

void copyFileData_c::stopDirectoryFiltering_f(QMutex* directoryFilterPtrMutexPtr_par)
{
    if (directoryFilterPtrMutexPtr_par not_eq nullptr)
    {
        QMutexLocker mutexLockerTmp(directoryFilterPtrMutexPtr_par);
        if (directoryFilterPtr_pro not_eq nullptr)
        {
            directoryFilterPtr_pro->stopFiltering_f();
        }
    }
    else
    {
        if (directoryFilterPtr_pro not_eq nullptr)
        {
            directoryFilterPtr_pro->stopFiltering_f();
        }
    }

}

bool copyFileData_c::copyHidden_f() const
{
    return copyHidden_pro;
}

void copyFileData_c::setCopyHidden_f(const bool copyHidden_par_con)
{
    copyHidden_pro = copyHidden_par_con;
}

QStringList copyFileData_c::sourceFilenameFullExtensions_f() const
{
    return sourceFilenameFullExtensions_pro;
}

QStringList copyFileData_c::sourceFilenameFullExtensionsParsed_f() const
{
    COPYPARSERETURNSTRINGLIST(sourceFilenameFullExtensions_pro);
}

void copyFileData_c::setSourceFilenameFullExtensions_f(const QStringList& sourceFilenameFullExtensions_par_con)
{
    sourceFilenameFullExtensions_pro = sourceFilenameFullExtensions_par_con;
}

bool copyFileData_c::navigateHidden_f() const
{
    return navigateHidden_pro;
}

void copyFileData_c::setNavigateHidden_f(const bool navigateHidden_par_con)
{
    navigateHidden_pro = navigateHidden_par_con;
}

bool copyFileData_c::copyEmptyDirectories_f() const
{
    return copyEmptyDirectories_pro;
}

void copyFileData_c::setCopyEmptyDirectories_f(const bool copyEmptyDirectories_par_con)
{
    copyEmptyDirectories_pro = copyEmptyDirectories_par_con;
}

bool copyFileData_c::noFilesCopiedIsError_f() const
{
    return noFilesCopiedIsError_pro;
}

void copyFileData_c::setNoFilesCopiedIsError_f(const bool noFilesCopiedIsError_par_con)
{
    noFilesCopiedIsError_pro = noFilesCopiedIsError_par_con;
}

copyFileData_c::transferType_ec copyFileData_c::transferType_f() const
{
    return transferType_pro;
}

void copyFileData_c::setTransferType_f(const transferType_ec transferType_par_con)
{
    transferType_pro = transferType_par_con;
}

copyFileData_c::destinationTreatment_ec copyFileData_c::destinationTreatment_f() const
{
    return destinationTreatment_pro;
}

void copyFileData_c::setDestinationTreatment_f(const destinationTreatment_ec& destinationTreatment_par_con)
{
    destinationTreatment_pro = destinationTreatment_par_con;
}

int_fast64_t copyFileData_c::bufferSize_f() const
{
    return bufferSize_pro;
}

void copyFileData_c::setBufferSize_f(const int_fast64_t& bufferSize_par_con)
{
    bufferSize_pro = bufferSize_par_con;
}

bool copyFileData_c::stopAllCopyOnFileCopyError_f() const
{
    return stopAllCopyOnFileCopyError_pro;
}

void copyFileData_c::setStopAllCopyOnFileCopyError_f(const bool stopAllCopyOnFileCopyError_par_con)
{
    stopAllCopyOnFileCopyError_pro = stopAllCopyOnFileCopyError_par_con;
}

copyFileData_c::copyFileData_c(
        const QString& sourcePath_par_con
        , const QString& destinationPath_par_con
        , const transferType_ec transferType_par_con
        , const destinationTreatment_ec destinationTreatment_par_con
        , const bool copyHidden_par_con
        , const QStringList& sourceFilenameRegexFilters_par_con
        , const QStringList& sourceFilenameFullExtensions_par_con
        , const bool nagivateSubdirectories_par_con
        , const bool navigateHidden_par_con
        , const bool copyEmptyDirectories_par_con
        , const bool createDestinationParent_par_con
        , const bool stopAllCopyOnFileCopyError_par_con
        , const bool noFilesCopiedIsError_par_con
        , const int_fast64_t bufferSize_par_con)
    : sourcePath_pro(sourcePath_par_con)
    , destinationPath_pro(destinationPath_par_con)
    , transferType_pro(transferType_par_con)
    , destinationTreatment_pro(destinationTreatment_par_con)
    , copyHidden_pro(copyHidden_par_con)
    , sourceFilenameRegexFilters_pro(sourceFilenameRegexFilters_par_con)
    , sourceFilenameFullExtensions_pro(sourceFilenameFullExtensions_par_con)
    , navigateSubdirectories_pro(nagivateSubdirectories_par_con)
    , navigateHidden_pro(navigateHidden_par_con)
    , copyEmptyDirectories_pro(copyEmptyDirectories_par_con)
    , createDestinationAndParents_pro(createDestinationParent_par_con)
    , stopAllCopyOnFileCopyError_pro(stopAllCopyOnFileCopyError_par_con)
    , noFilesCopiedIsError_pro(noFilesCopiedIsError_par_con)
    , bufferSize_pro(bufferSize_par_con)
{}

//copyFile_c::copyFileAction_c(const copyFileAction_c& from_par_con)
//    : sourcePath_pri(from_par_con.sourcePath_pri)
//    , destinationPath_pri(from_par_con.destinationPath_pri)
//    , transferType_pri(from_par_con.transferType_pri)
//    , destinationTreatment_pri(from_par_con.destinationTreatment_pri)
//    , copyHidden_pri(from_par_con.copyHidden_pri)
//    , sourceFilenameRegexFilters_pri(from_par_con.sourceFilenameRegexFilters_pri)
//    , sourceFilenameFullExtensions_pri(from_par_con.sourceFilenameFullExtensions_pri)
//    , navigateSubdirectories_pri(from_par_con.navigateSubdirectories_pri)
//    , navigateHidden_pri(from_par_con.navigateHidden_pri)
//    , copyEmptyDirectories_pri(from_par_con.copyEmptyDirectories_pri)
//    , createDestinationAndParents_pri(from_par_con.createDestinationAndParents_pri)
//    , stopAllCopyOnFileCopyError_pri(from_par_con.stopAllCopyOnFileCopyError_pri)
//    , noFilesCopiedIsError_pri(from_par_con.noFilesCopiedIsError_pri)
//    , bufferSize_pri(from_par_con.bufferSize_pri)
//{}

//copyFile_c::copyFileAction_c(copyFileAction_c& from_par)
//    : sourcePath_pri(from_par.sourcePath_pri)
//    , destinationPath_pri(from_par.destinationPath_pri)
//    , transferType_pri(from_par.transferType_pri)
//    , destinationTreatment_pri(from_par.destinationTreatment_pri)
//    , copyHidden_pri(from_par.copyHidden_pri)
//    , sourceFilenameRegexFilters_pri(from_par.sourceFilenameRegexFilters_pri)
//    , sourceFilenameFullExtensions_pri(from_par.sourceFilenameFullExtensions_pri)
//    , navigateSubdirectories_pri(from_par.navigateSubdirectories_pri)
//    , navigateHidden_pri(from_par.navigateHidden_pri)
//    , copyEmptyDirectories_pri(from_par.copyEmptyDirectories_pri)
//    , createDestinationAndParents_pri(from_par.createDestinationAndParents_pri)
//    , stopAllCopyOnFileCopyError_pri(from_par.stopAllCopyOnFileCopyError_pri)
//    , noFilesCopiedIsError_pri(from_par.noFilesCopiedIsError_pri)
//    , bufferSize_pri(from_par.bufferSize_pri)
//{}

//copyFile_c::copyFileAction_c(copyFileAction_c&& from_par) noexcept
//    : sourcePath_pri(std::move(from_par.sourcePath_pri))
//    , destinationPath_pri(std::move(from_par.destinationPath_pri))
//    , transferType_pri(from_par.transferType_pri)
//    , destinationTreatment_pri(from_par.destinationTreatment_pri)
//    , copyHidden_pri(from_par.copyHidden_pri)
//    , sourceFilenameRegexFilters_pri(std::move(from_par.sourceFilenameRegexFilters_pri))
//    , sourceFilenameFullExtensions_pri(std::move(from_par.sourceFilenameFullExtensions_pri))
//    , navigateSubdirectories_pri(from_par.navigateSubdirectories_pri)
//    , navigateHidden_pri(from_par.navigateHidden_pri)
//    , copyEmptyDirectories_pri(from_par.copyEmptyDirectories_pri)
//    , createDestinationAndParents_pri(from_par.createDestinationAndParents_pri)
//    , stopAllCopyOnFileCopyError_pri(from_par.stopAllCopyOnFileCopyError_pri)
//    , noFilesCopiedIsError_pri(from_par.noFilesCopiedIsError_pri)
//    , bufferSize_pri(from_par.bufferSize_pri)
//{
//    from_par.directoryFilterPtr_pri = nullptr;
//}

//copyFileAction_c&copyFile_c::operator=(const copyFileAction_c& from_par_con)
//{
//    sourcePath_pri = from_par_con.sourcePath_pri;
//    destinationPath_pri = from_par_con.destinationPath_pri;
//    transferType_pri = from_par_con.transferType_pri;
//    destinationTreatment_pri = from_par_con.destinationTreatment_pri;
//    copyHidden_pri = from_par_con.copyHidden_pri;
//    sourceFilenameRegexFilters_pri = from_par_con.sourceFilenameRegexFilters_pri;
//    sourceFilenameFullExtensions_pri = from_par_con.sourceFilenameFullExtensions_pri;
//    navigateSubdirectories_pri = from_par_con.navigateSubdirectories_pri;
//    navigateHidden_pri = from_par_con.navigateHidden_pri;
//    copyEmptyDirectories_pri = from_par_con.copyEmptyDirectories_pri;
//    createDestinationAndParents_pri = from_par_con.createDestinationAndParents_pri;
//    stopAllCopyOnFileCopyError_pri = from_par_con.stopAllCopyOnFileCopyError_pri;
//    noFilesCopiedIsError_pri = from_par_con.noFilesCopiedIsError_pri;
//    bufferSize_pri = from_par_con.bufferSize_pri;

//    return *this;
//}

//copyFileAction_c&copyFile_c::operator=(copyFileAction_c& from_par)
//{
//    sourcePath_pri = from_par.sourcePath_pri;
//    destinationPath_pri = from_par.destinationPath_pri;
//    transferType_pri = from_par.transferType_pri;
//    destinationTreatment_pri = from_par.destinationTreatment_pri;
//    copyHidden_pri = from_par.copyHidden_pri;
//    sourceFilenameRegexFilters_pri = from_par.sourceFilenameRegexFilters_pri;
//    sourceFilenameFullExtensions_pri = from_par.sourceFilenameFullExtensions_pri;
//    navigateSubdirectories_pri = from_par.navigateSubdirectories_pri;
//    navigateHidden_pri = from_par.navigateHidden_pri;
//    copyEmptyDirectories_pri = from_par.copyEmptyDirectories_pri;
//    createDestinationAndParents_pri = from_par.createDestinationAndParents_pri;
//    stopAllCopyOnFileCopyError_pri = from_par.stopAllCopyOnFileCopyError_pri;
//    noFilesCopiedIsError_pri = from_par.noFilesCopiedIsError_pri;
//    bufferSize_pri = from_par.bufferSize_pri;

//    return *this;
//}

//copyFileAction_c&copyFile_c::operator=(copyFileAction_c&& from_par) noexcept
//{
//    sourcePath_pri = std::move(from_par.sourcePath_pri);
//    destinationPath_pri = std::move(from_par.destinationPath_pri);
//    transferType_pri = from_par.transferType_pri;
//    destinationTreatment_pri = from_par.destinationTreatment_pri;
//    copyHidden_pri = from_par.copyHidden_pri;
//    sourceFilenameRegexFilters_pri = std::move(from_par.sourceFilenameRegexFilters_pri);
//    sourceFilenameFullExtensions_pri = std::move(from_par.sourceFilenameFullExtensions_pri);
//    navigateSubdirectories_pri = from_par.navigateSubdirectories_pri;
//    navigateHidden_pri = from_par.navigateHidden_pri;
//    copyEmptyDirectories_pri = from_par.copyEmptyDirectories_pri;
//    createDestinationAndParents_pri = from_par.createDestinationAndParents_pri;
//    stopAllCopyOnFileCopyError_pri = from_par.stopAllCopyOnFileCopyError_pri;
//    noFilesCopiedIsError_pri = from_par.noFilesCopiedIsError_pri;
//    bufferSize_pri = from_par.bufferSize_pri;

//    directoryFilterPtr_pri = nullptr;

//    return *this;
//}

copyFileAction_c::copyFileAction_c(
        const actionData_c& actionData_par_con
        , const copyFileData_c& copyFile_par_con)
    : action_c(actionData_par_con)
    , copyFileData_c(copyFile_par_con)
{
}

void copyFileAction_c::derivedWrite_f(QJsonObject& json_par_ref) const
{
    json_par_ref["sourcePath"] = sourcePath_pro;
    json_par_ref["destinationPath"] = destinationPath_pro;
    json_par_ref["transferType"] = transferTypeToStrUMap_sta_con.at(transferType_pro);
    json_par_ref["destinationTreatment"] = destinationTreatmentToStrUMap_sta_con.at(destinationTreatment_pro);
    json_par_ref["copyHidden"] = copyHidden_pro;
    if (not sourceFilenameRegexFilters_pro.empty())
    {
        QJsonArray sourceRegexFiltersArray;
        for (const QString& regexFilter_ite_con : sourceFilenameRegexFilters_pro)
        {
            sourceRegexFiltersArray.append(QJsonValue(regexFilter_ite_con));
        }
        json_par_ref["sourceRegexFilters"] = sourceRegexFiltersArray;
    }
    if (not sourceFilenameFullExtensions_pro.empty())
    {
        QJsonArray filenameFullExtensionsArray;
        for (const QString& fileFullnameExtension_ite_con : sourceFilenameFullExtensions_pro)
        {
            filenameFullExtensionsArray.append(QJsonValue(fileFullnameExtension_ite_con));
        }
        json_par_ref["filenameFullExtensions"] = filenameFullExtensionsArray;
    }
    json_par_ref["navigateSubdirectories"] = navigateSubdirectories_pro;
    json_par_ref["navigateHidden"] = navigateHidden_pro;
    json_par_ref["copyEmptyDirectories"] = copyEmptyDirectories_pro;
    json_par_ref["createDestinationAndParents"] = createDestinationAndParents_pro;
    json_par_ref["stopAllCopyOnFileCopyError"] = stopAllCopyOnFileCopyError_pro;
    json_par_ref["noFilesCopiedIsError"] = noFilesCopiedIsError_pro;
    json_par_ref["bufferSize"] = QString::number(bufferSize_pro);
}

void copyFileAction_c::derivedRead_f(const QJsonObject& json_par_con)
{
    sourcePath_pro = json_par_con["sourcePath"].toString();
    destinationPath_pro = json_par_con["destinationPath"].toString();
    transferType_pro = strToTransferTypeMap_sta_con.value(json_par_con["transferType"].toString().toLower());
    destinationTreatment_pro = strToDestinationTreatmentMap_sta_con.value(json_par_con["destinationTreatment"].toString().toLower());
    copyHidden_pro = json_par_con["copyHidden"].toBool();
    if (not json_par_con["sourceRegexFilters"].isUndefined())
    {
        QJsonArray jsonArraySourceRegexFiltersTmp(json_par_con["sourceRegexFilters"].toArray());
        if (not jsonArraySourceRegexFiltersTmp.isEmpty())
        {
            sourceFilenameRegexFilters_pro.reserve(jsonArraySourceRegexFiltersTmp.size());
            for (const auto jsonArrayItem_ite_con : jsonArraySourceRegexFiltersTmp)
            {
                sourceFilenameRegexFilters_pro.append(jsonArrayItem_ite_con.toString());
            }
        }
    }
    if (not json_par_con["filenameFullExtensions"].isUndefined())
    {
        QJsonArray jsonArrayFilenameFullExtensionsTmp(json_par_con["filenameFullExtensions"].toArray());
        if (not jsonArrayFilenameFullExtensionsTmp.isEmpty())
        {
            sourceFilenameFullExtensions_pro.reserve(jsonArrayFilenameFullExtensionsTmp.size());
            for (const auto jsonArrayItem_ite_con : jsonArrayFilenameFullExtensionsTmp)
            {
                sourceFilenameFullExtensions_pro.append(jsonArrayItem_ite_con.toString());
            }
        }
    }

    //all the isbool is to not replace the default value from the class defaults,
    //because toBool() defaults to false when the field is not found or the conversion fails
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        navigateSubdirectories_pro = json_par_con["navigateSubdirectories"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        navigateHidden_pro = json_par_con["navigateHidden"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        copyEmptyDirectories_pro = json_par_con["copyEmptyDirectories"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        createDestinationAndParents_pro = json_par_con["createDestinationAndParents"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        stopAllCopyOnFileCopyError_pro = json_par_con["stopAllCopyOnFileCopyError"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        noFilesCopiedIsError_pro = json_par_con["noFilesCopiedIsError"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isUndefined())
    {
        bool goodConversionTmp(false);
        int_fast64_t conversionResultTmp(json_par_con["bufferSize"].toString().toLongLong(std::addressof(goodConversionTmp)));
        if (goodConversionTmp)
        {
            bufferSize_pro = conversionResultTmp;
        }
    }
}

action_c* copyFileAction_c::derivedClone_f() const
{
    //slice and dice
    copyFileData_c copyFileDataTmp(*this);
    actionData_c actionDataTmp(*this);
    return new copyFileAction_c(actionDataTmp, copyFileDataTmp);
}

baseActionExecution_c* copyFileAction_c::createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par)
{
    return new copyFileActionExecution_c(actionDataExecutionResult_ptr_par, this);
}

actionType_ec copyFileAction_c::type_f() const
{
    return actionType_ec::copyFile;
}

QString copyFileAction_c::typeStr_f() const
{
    return actionTypeToStrUMap_ext_con.at(actionType_ec::copyFile);
}

bool copyFileData_c::isValid_f(QString* errorStrPtr_par) const
{
    bool isValidResultTmp(false);
    while (true)
    {
        if (sourcePath_pro.isEmpty())
        {
            APPENDSTRPTR(errorStrPtr_par, "Source path is empty");
            break;
        }
        if (transferType_pro == transferType_ec::empty)
        {
            APPENDSTRPTR(errorStrPtr_par, "Empty transfer type");
            break;
        }
        if (destinationTreatment_pro == destinationTreatment_ec::empty)
        {
            APPENDSTRPTR(errorStrPtr_par, "Empty destination treatment");
            break;
        }

        if (transferType_pro == transferType_ec::trueMove
            and equalOnce_ft(destinationTreatment_pro, destinationTreatment_ec::lazyResume, destinationTreatment_ec::tryStupidResume))
        {
            APPENDSTRPTR(errorStrPtr_par, R"("True move" is not compatible with any "resume" destination treatment)");
            break;
        }

        isValidResultTmp = true;
        break;
    }
    return isValidResultTmp;
}
