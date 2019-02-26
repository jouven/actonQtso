#include "copyFile.hpp"

#include "essentialQtso/macros.hpp"
#include "comuso/practicalTemplates.hpp"

#include <QDir>
#include <QFileInfo>
#include <QJsonArray>

const QMap<QString, copyFileAction_c::transferType_ec> copyFileAction_c::strToTransferTypeMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"copy", copyFileAction_c::transferType_ec::copy}
    , {	"move", copyFileAction_c::transferType_ec::move}
    , {	"truemove", copyFileAction_c::transferType_ec::trueMove}
});

const std::unordered_map<copyFileAction_c::transferType_ec, QString> copyFileAction_c::transferTypeToStrUMap_sta_con(
{
    {	copyFileAction_c::transferType_ec::copy, "copy" }
    , {	copyFileAction_c::transferType_ec::move, "move" }
    , {	copyFileAction_c::transferType_ec::trueMove, "trueMove" }

});


const QMap<QString, copyFileAction_c::destinationTreatment_ec> copyFileAction_c::strToDestinationTreatmentMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"overwrite", copyFileAction_c::destinationTreatment_ec::overwrite}
    , {	"nooverwrite", copyFileAction_c::destinationTreatment_ec::noOverwrite}
    , {	"trystupidresume", copyFileAction_c::destinationTreatment_ec::tryStupidResume}
    , {	"lazyresume", copyFileAction_c::destinationTreatment_ec::lazyResume}
    , {	"overwriteiserror", copyFileAction_c::destinationTreatment_ec::overwriteIsError}
    , {	"overwriteonlyifdifferent", copyFileAction_c::destinationTreatment_ec::overwriteOnlyIfDifferent}
    , {	"overwriteiserroronlyifdifferent", copyFileAction_c::destinationTreatment_ec::overwriteIsErrorOnlyIfDifferent}
});

const std::unordered_map<copyFileAction_c::destinationTreatment_ec, QString> copyFileAction_c::destinationTreatmentToStrUMap_sta_con(
{
    {	copyFileAction_c::destinationTreatment_ec::overwrite, "overwrite" }
    , {	copyFileAction_c::destinationTreatment_ec::noOverwrite, "noOverwrite" }
    , {	copyFileAction_c::destinationTreatment_ec::tryStupidResume, "tryStupidResume" }
    , {	copyFileAction_c::destinationTreatment_ec::lazyResume, "lazyResume" }
    , {	copyFileAction_c::destinationTreatment_ec::overwriteIsError, "overwriteIsError" }
    , {	copyFileAction_c::destinationTreatment_ec::overwriteOnlyIfDifferent, "overwriteOnlyIfDifferent" }
    , {	copyFileAction_c::destinationTreatment_ec::overwriteIsErrorOnlyIfDifferent, "overwriteIsErrorOnlyIfDifferent" }
});

QString copyFileAction_c::sourcePath_f() const
{
    return sourcePath_pri;
}

void copyFileAction_c::setSourcePath_f(const QString& sourcePath_par_con)
{
    sourcePath_pri = sourcePath_par_con;
}

QString copyFileAction_c::destinationPath_f() const
{
    return destinationPath_pri;
}

void copyFileAction_c::setDestinationPath_f(const QString& destinationPath_par_con)
{
    destinationPath_pri = destinationPath_par_con;
}

bool copyFileAction_c::navigateSubdirectories_f() const
{
    return navigateSubdirectories_pri;
}

void copyFileAction_c::setNavigateSubdirectories_f(const bool includeSubdirectories_par_con)
{
    navigateSubdirectories_pri = includeSubdirectories_par_con;
}

QStringList copyFileAction_c::sourceFilenameRegexFilters_f() const
{
    return sourceFilenameRegexFilters_pri;
}

void copyFileAction_c::setSourceFilenameRegexFilters_f(const QStringList& sourceFilenameRegexFilters_par_con)
{
    sourceFilenameRegexFilters_pri = sourceFilenameRegexFilters_par_con;
}

bool copyFileAction_c::createDestinationAndParents_f() const
{
    return createDestinationAndParents_pri;
}

void copyFileAction_c::setCreateDestinationAndParents_f(const bool createDestinationParent_par_con)
{
    createDestinationAndParents_pri = createDestinationParent_par_con;
}

std::vector<QString> copyFileAction_c::testSourceFileList_f(QString* error_ptr)
{
    std::vector<QString> resultTmp;
    while (isValid_f(error_ptr))
    {
        //ignore destination options
        if (QFileInfo::exists(sourcePath_pri))
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

        QFileInfo sourceFileInfoTmp(sourcePath_pri);
        if (sourceFileInfoTmp.isDir())
        {
            filterOptions_s filterOptionsTmp;
            filterOptionsTmp.navigateSubdirectories_pub = navigateSubdirectories_pri;
            filterOptionsTmp.useAbsolutePaths_pub = sourceFileInfoTmp.isAbsolute();
            filterOptionsTmp.listFiles_pub = true;
            filterOptionsTmp.listHidden_pub = copyHidden_pri;
            filterOptionsTmp.navigateHiddenDirectories_pub = copyHidden_pri;
            filterOptionsTmp.listDirectories_pub = false;
            filterOptionsTmp.filenameRegexFilters_pub = sourceFilenameRegexFilters_pri;
            filterOptionsTmp.listEmptyDirectories_pub = copyEmptyDirectories_pri;
            filterOptionsTmp.filenameFullExtensions_pub = sourceFilenameFullExtensions_pri;

            directoryFilter_c directoryFilterTmp(sourcePath_pri, filterOptionsTmp);
            {
                QMutexLocker mutexLockerTmp(std::addressof(directoryFilterPtrMutex_pri));
                directoryFilterPtr_pri = std::addressof(directoryFilterTmp);
            }
            resultTmp = directoryFilterTmp.filter_f();
            if (error_ptr not_eq nullptr and directoryFilterTmp.anyError_f())
            {
                error_ptr->append(directoryFilterTmp.getError_f());
            }
            {
                QMutexLocker mutexLockerTmp(std::addressof(directoryFilterPtrMutex_pri));
                directoryFilterPtr_pri = nullptr;
            }

            break;
        }

        if (sourceFileInfoTmp.isFile())
        {
            resultTmp.emplace_back(sourcePath_pri);
            break;
        }
        break;
    }
    return resultTmp;
}

void copyFileAction_c::stopDirectoryFiltering_f()
{
    QMutexLocker mutexLockerTmp(std::addressof(directoryFilterPtrMutex_pri));
    if (directoryFilterPtr_pri not_eq nullptr)
    {
        directoryFilterPtr_pri->stopFiltering_f();
    }
}

bool copyFileAction_c::copyHidden_f() const
{
    return copyHidden_pri;
}

void copyFileAction_c::setCopyHidden_f(const bool copyHidden_par_con)
{
    copyHidden_pri = copyHidden_par_con;
}

QStringList copyFileAction_c::sourceFilenameFullExtensions_f() const
{
    return sourceFilenameFullExtensions_pri;
}

void copyFileAction_c::setSourceFilenameFullExtensions_f(const QStringList& sourceFilenameFullExtensions_par_con)
{
    sourceFilenameFullExtensions_pri = sourceFilenameFullExtensions_par_con;
}

bool copyFileAction_c::navigateHidden_f() const
{
    return navigateHidden_pri;
}

void copyFileAction_c::setNavigateHidden_f(const bool navigateHidden_par_con)
{
    navigateHidden_pri = navigateHidden_par_con;
}

bool copyFileAction_c::copyEmptyDirectories_f() const
{
    return copyEmptyDirectories_pri;
}

void copyFileAction_c::setCopyEmptyDirectories_f(const bool copyEmptyDirectories_par_con)
{
    copyEmptyDirectories_pri = copyEmptyDirectories_par_con;
}

bool copyFileAction_c::noFilesCopiedIsError_f() const
{
    return noFilesCopiedIsError_pri;
}

void copyFileAction_c::setNoFilesCopiedIsError_f(const bool noFilesCopiedIsError_par_con)
{
    noFilesCopiedIsError_pri = noFilesCopiedIsError_par_con;
}

copyFileAction_c::transferType_ec copyFileAction_c::transferType_f() const
{
    return transferType_pri;
}

void copyFileAction_c::setTransferType_f(const transferType_ec transferType_par_con)
{
    transferType_pri = transferType_par_con;
}

copyFileAction_c::destinationTreatment_ec copyFileAction_c::destinationTreatment_f() const
{
    return destinationTreatment_pri;
}

void copyFileAction_c::setDestinationTreatment_f(const destinationTreatment_ec& destinationTreatment_par_con)
{
    destinationTreatment_pri = destinationTreatment_par_con;
}

int_fast64_t copyFileAction_c::bufferSize_f() const
{
    return bufferSize_pri;
}

void copyFileAction_c::setBufferSize_f(const int_fast64_t& bufferSize_par_con)
{
    bufferSize_pri = bufferSize_par_con;
}

bool copyFileAction_c::stopAllCopyOnFileCopyError_f() const
{
    return stopAllCopyOnFileCopyError_pri;
}

void copyFileAction_c::setStopAllCopyOnFileCopyError_f(const bool stopAllCopyOnFileCopyError_par_con)
{
    stopAllCopyOnFileCopyError_pri = stopAllCopyOnFileCopyError_par_con;
}

copyFileAction_c::copyFileAction_c(
        const QString& sourcePath_par_con
        , const QString& destinationPath_par_con
        , const transferType_ec transferType_par_con
        , const destinationTreatment_ec destinationTreatment_par_con
        , const bool copyHidden_par_con
        , const QStringList& sourceRegexFilters_par_con
        , const QStringList& filenameFullExtensions_par_con
        , const bool nagivateSubdirectories_par_con
        , const bool navigateHidden_par_con
        , const bool copyEmptyDirectories_par_con
        , const bool createDestinationParent_par_con
        , const bool stopAllCopyOnFileCopyError_par_con
        , const bool noFilesCopiedIsError_par_con
        , const int_fast64_t bufferSize_par_con)
    : sourcePath_pri(sourcePath_par_con)
    , destinationPath_pri(destinationPath_par_con)
    , transferType_pri(transferType_par_con)
    , destinationTreatment_pri(destinationTreatment_par_con)
    , copyHidden_pri(copyHidden_par_con)
    , sourceFilenameRegexFilters_pri(sourceRegexFilters_par_con)
    , sourceFilenameFullExtensions_pri(filenameFullExtensions_par_con)
    , navigateSubdirectories_pri(nagivateSubdirectories_par_con)
    , navigateHidden_pri(navigateHidden_par_con)
    , copyEmptyDirectories_pri(copyEmptyDirectories_par_con)
    , createDestinationAndParents_pri(createDestinationParent_par_con)
    , stopAllCopyOnFileCopyError_pri(stopAllCopyOnFileCopyError_par_con)
    , noFilesCopiedIsError_pri(noFilesCopiedIsError_par_con)
    , bufferSize_pri(bufferSize_par_con)
{}

copyFileAction_c::copyFileAction_c(const copyFileAction_c& from_par_con)
    : sourcePath_pri(from_par_con.sourcePath_pri)
    , destinationPath_pri(from_par_con.destinationPath_pri)
    , transferType_pri(from_par_con.transferType_pri)
    , destinationTreatment_pri(from_par_con.destinationTreatment_pri)
    , copyHidden_pri(from_par_con.copyHidden_pri)
    , sourceFilenameRegexFilters_pri(from_par_con.sourceFilenameRegexFilters_pri)
    , sourceFilenameFullExtensions_pri(from_par_con.sourceFilenameFullExtensions_pri)
    , navigateSubdirectories_pri(from_par_con.navigateSubdirectories_pri)
    , navigateHidden_pri(from_par_con.navigateHidden_pri)
    , copyEmptyDirectories_pri(from_par_con.copyEmptyDirectories_pri)
    , createDestinationAndParents_pri(from_par_con.createDestinationAndParents_pri)
    , stopAllCopyOnFileCopyError_pri(from_par_con.stopAllCopyOnFileCopyError_pri)
    , noFilesCopiedIsError_pri(from_par_con.noFilesCopiedIsError_pri)
    , bufferSize_pri(from_par_con.bufferSize_pri)
{}

copyFileAction_c::copyFileAction_c(copyFileAction_c& from_par)
    : sourcePath_pri(from_par.sourcePath_pri)
    , destinationPath_pri(from_par.destinationPath_pri)
    , transferType_pri(from_par.transferType_pri)
    , destinationTreatment_pri(from_par.destinationTreatment_pri)
    , copyHidden_pri(from_par.copyHidden_pri)
    , sourceFilenameRegexFilters_pri(from_par.sourceFilenameRegexFilters_pri)
    , sourceFilenameFullExtensions_pri(from_par.sourceFilenameFullExtensions_pri)
    , navigateSubdirectories_pri(from_par.navigateSubdirectories_pri)
    , navigateHidden_pri(from_par.navigateHidden_pri)
    , copyEmptyDirectories_pri(from_par.copyEmptyDirectories_pri)
    , createDestinationAndParents_pri(from_par.createDestinationAndParents_pri)
    , stopAllCopyOnFileCopyError_pri(from_par.stopAllCopyOnFileCopyError_pri)
    , noFilesCopiedIsError_pri(from_par.noFilesCopiedIsError_pri)
    , bufferSize_pri(from_par.bufferSize_pri)
{}

copyFileAction_c::copyFileAction_c(copyFileAction_c&& from_par) noexcept
    : sourcePath_pri(std::move(from_par.sourcePath_pri))
    , destinationPath_pri(std::move(from_par.destinationPath_pri))
    , transferType_pri(from_par.transferType_pri)
    , destinationTreatment_pri(from_par.destinationTreatment_pri)
    , copyHidden_pri(from_par.copyHidden_pri)
    , sourceFilenameRegexFilters_pri(std::move(from_par.sourceFilenameRegexFilters_pri))
    , sourceFilenameFullExtensions_pri(std::move(from_par.sourceFilenameFullExtensions_pri))
    , navigateSubdirectories_pri(from_par.navigateSubdirectories_pri)
    , navigateHidden_pri(from_par.navigateHidden_pri)
    , copyEmptyDirectories_pri(from_par.copyEmptyDirectories_pri)
    , createDestinationAndParents_pri(from_par.createDestinationAndParents_pri)
    , stopAllCopyOnFileCopyError_pri(from_par.stopAllCopyOnFileCopyError_pri)
    , noFilesCopiedIsError_pri(from_par.noFilesCopiedIsError_pri)
    , bufferSize_pri(from_par.bufferSize_pri)
{
    from_par.directoryFilterPtr_pri = nullptr;
}

copyFileAction_c&copyFileAction_c::operator=(const copyFileAction_c& from_par_con)
{
    sourcePath_pri = from_par_con.sourcePath_pri;
    destinationPath_pri = from_par_con.destinationPath_pri;
    transferType_pri = from_par_con.transferType_pri;
    destinationTreatment_pri = from_par_con.destinationTreatment_pri;
    copyHidden_pri = from_par_con.copyHidden_pri;
    sourceFilenameRegexFilters_pri = from_par_con.sourceFilenameRegexFilters_pri;
    sourceFilenameFullExtensions_pri = from_par_con.sourceFilenameFullExtensions_pri;
    navigateSubdirectories_pri = from_par_con.navigateSubdirectories_pri;
    navigateHidden_pri = from_par_con.navigateHidden_pri;
    copyEmptyDirectories_pri = from_par_con.copyEmptyDirectories_pri;
    createDestinationAndParents_pri = from_par_con.createDestinationAndParents_pri;
    stopAllCopyOnFileCopyError_pri = from_par_con.stopAllCopyOnFileCopyError_pri;
    noFilesCopiedIsError_pri = from_par_con.noFilesCopiedIsError_pri;
    bufferSize_pri = from_par_con.bufferSize_pri;

    return *this;
}

copyFileAction_c&copyFileAction_c::operator=(copyFileAction_c& from_par)
{
    sourcePath_pri = from_par.sourcePath_pri;
    destinationPath_pri = from_par.destinationPath_pri;
    transferType_pri = from_par.transferType_pri;
    destinationTreatment_pri = from_par.destinationTreatment_pri;
    copyHidden_pri = from_par.copyHidden_pri;
    sourceFilenameRegexFilters_pri = from_par.sourceFilenameRegexFilters_pri;
    sourceFilenameFullExtensions_pri = from_par.sourceFilenameFullExtensions_pri;
    navigateSubdirectories_pri = from_par.navigateSubdirectories_pri;
    navigateHidden_pri = from_par.navigateHidden_pri;
    copyEmptyDirectories_pri = from_par.copyEmptyDirectories_pri;
    createDestinationAndParents_pri = from_par.createDestinationAndParents_pri;
    stopAllCopyOnFileCopyError_pri = from_par.stopAllCopyOnFileCopyError_pri;
    noFilesCopiedIsError_pri = from_par.noFilesCopiedIsError_pri;
    bufferSize_pri = from_par.bufferSize_pri;

    return *this;
}

copyFileAction_c&copyFileAction_c::operator=(copyFileAction_c&& from_par) noexcept
{
    sourcePath_pri = std::move(from_par.sourcePath_pri);
    destinationPath_pri = std::move(from_par.destinationPath_pri);
    transferType_pri = from_par.transferType_pri;
    destinationTreatment_pri = from_par.destinationTreatment_pri;
    copyHidden_pri = from_par.copyHidden_pri;
    sourceFilenameRegexFilters_pri = std::move(from_par.sourceFilenameRegexFilters_pri);
    sourceFilenameFullExtensions_pri = std::move(from_par.sourceFilenameFullExtensions_pri);
    navigateSubdirectories_pri = from_par.navigateSubdirectories_pri;
    navigateHidden_pri = from_par.navigateHidden_pri;
    copyEmptyDirectories_pri = from_par.copyEmptyDirectories_pri;
    createDestinationAndParents_pri = from_par.createDestinationAndParents_pri;
    stopAllCopyOnFileCopyError_pri = from_par.stopAllCopyOnFileCopyError_pri;
    noFilesCopiedIsError_pri = from_par.noFilesCopiedIsError_pri;
    bufferSize_pri = from_par.bufferSize_pri;

    directoryFilterPtr_pri = nullptr;

    return *this;
}

void copyFileAction_c::write_f(QJsonObject& json_par_ref) const
{
    json_par_ref["sourcePath"] = sourcePath_pri;
    json_par_ref["destinationPath"] = destinationPath_pri;
    json_par_ref["transferType"] = transferTypeToStrUMap_sta_con.at(transferType_pri);
    json_par_ref["destinationTreatment"] = destinationTreatmentToStrUMap_sta_con.at(destinationTreatment_pri);
    json_par_ref["copyHidden"] = copyHidden_pri;
    if (not sourceFilenameRegexFilters_pri.empty())
    {
        QJsonArray sourceRegexFiltersArray;
        for (const QString& regexFilter_ite_con : sourceFilenameRegexFilters_pri)
        {
            sourceRegexFiltersArray.append(QJsonValue(regexFilter_ite_con));
        }
        json_par_ref["sourceRegexFilters"] = sourceRegexFiltersArray;
    }
    if (not sourceFilenameFullExtensions_pri.empty())
    {
        QJsonArray filenameFullExtensionsArray;
        for (const QString& fileFullnameExtension_ite_con : sourceFilenameFullExtensions_pri)
        {
            filenameFullExtensionsArray.append(QJsonValue(fileFullnameExtension_ite_con));
        }
        json_par_ref["filenameFullExtensions"] = filenameFullExtensionsArray;
    }
    json_par_ref["navigateSubdirectories"] = navigateSubdirectories_pri;
    json_par_ref["navigateHidden"] = navigateHidden_pri;
    json_par_ref["copyEmptyDirectories"] = copyEmptyDirectories_pri;
    json_par_ref["createDestinationAndParents"] = createDestinationAndParents_pri;
    json_par_ref["stopAllCopyOnFileCopyError"] = stopAllCopyOnFileCopyError_pri;
    json_par_ref["noFilesCopiedIsError"] = noFilesCopiedIsError_pri;
    json_par_ref["bufferSize"] = QString::number(bufferSize_pri);
}

void copyFileAction_c::read_f(const QJsonObject& json_par_con)
{
    sourcePath_pri = json_par_con["sourcePath"].toString();
    destinationPath_pri = json_par_con["destinationPath"].toString();
    transferType_pri = strToTransferTypeMap_sta_con.value(json_par_con["transferType"].toString().toLower());
    destinationTreatment_pri = strToDestinationTreatmentMap_sta_con.value(json_par_con["destinationTreatment"].toString().toLower());
    copyHidden_pri = json_par_con["copyHidden"].toBool();
    if (not json_par_con["sourceRegexFilters"].isUndefined())
    {
        QJsonArray jsonArraySourceRegexFiltersTmp(json_par_con["sourceRegexFilters"].toArray());
        if (not jsonArraySourceRegexFiltersTmp.isEmpty())
        {
            sourceFilenameRegexFilters_pri.reserve(jsonArraySourceRegexFiltersTmp.size());
            for (const auto jsonArrayItem_ite_con : jsonArraySourceRegexFiltersTmp)
            {
                sourceFilenameRegexFilters_pri.append(jsonArrayItem_ite_con.toString());
            }
        }
    }
    if (not json_par_con["filenameFullExtensions"].isUndefined())
    {
        QJsonArray jsonArrayFilenameFullExtensionsTmp(json_par_con["filenameFullExtensions"].toArray());
        if (not jsonArrayFilenameFullExtensionsTmp.isEmpty())
        {
            sourceFilenameFullExtensions_pri.reserve(jsonArrayFilenameFullExtensionsTmp.size());
            for (const auto jsonArrayItem_ite_con : jsonArrayFilenameFullExtensionsTmp)
            {
                sourceFilenameFullExtensions_pri.append(jsonArrayItem_ite_con.toString());
            }
        }
    }

    //all the isbool is to not replace the default value from the class defaults,
    //because toBool() defaults to false when the field is not found or the conversion fails
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        navigateSubdirectories_pri = json_par_con["navigateSubdirectories"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        navigateHidden_pri = json_par_con["navigateHidden"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        copyEmptyDirectories_pri = json_par_con["copyEmptyDirectories"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        createDestinationAndParents_pri = json_par_con["createDestinationAndParents"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        stopAllCopyOnFileCopyError_pri = json_par_con["stopAllCopyOnFileCopyError"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isBool())
    {
        noFilesCopiedIsError_pri = json_par_con["noFilesCopiedIsError"].toBool();
    }
    if (not json_par_con["navigateSubdirectories"].isUndefined())
    {
        bool goodConversionTmp(false);
        int_fast64_t conversionResultTmp(json_par_con["bufferSize"].toString().toLongLong(std::addressof(goodConversionTmp)));
        if (goodConversionTmp)
        {
            bufferSize_pri = conversionResultTmp;
        }
    }
}

bool copyFileAction_c::isValid_f(QString* errorStrPtr_par) const
{
    bool isValidResultTmp(false);
    while (true)
    {
        if (sourcePath_pri.isEmpty())
        {
            APPENDSTRPTR(errorStrPtr_par, "Source path is empty");
            break;
        }
        if (transferType_pri == transferType_ec::empty)
        {
            APPENDSTRPTR(errorStrPtr_par, "Empty transfer type");
            break;
        }
        if (destinationTreatment_pri == destinationTreatment_ec::empty)
        {
            APPENDSTRPTR(errorStrPtr_par, "Empty destination treatment");
            break;
        }

        if (transferType_pri == transferType_ec::trueMove
            and equalOnce_ft(destinationTreatment_pri, destinationTreatment_ec::lazyResume, destinationTreatment_ec::tryStupidResume))
        {
            APPENDSTRPTR(errorStrPtr_par, R"("True move" is not compatible with any "resume" destination treatment)");
            break;
        }

        isValidResultTmp = true;
        break;
    }
    return isValidResultTmp;
}
