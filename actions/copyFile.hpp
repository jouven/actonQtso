#ifndef ACTONQTSO_COPYFILE_HPP
#define ACTONQTSO_COPYFILE_HPP

#include "../actionData.hpp"

#include "../crossPlatformMacros.hpp"

#include "filterDirectoryQtso/filterDirectory.hpp"
#include "textQtso/text.hpp"

#include <QString>
#include <QStringList>

#include <QMap>

#include <vector>
#include <unordered_map>

class QJsonObject;
class QMutex;
class stringParserMap_c;

//the class is split in two,
//to allow copying and not being forced to use "new",
//this makes working with copyFile classes less cumbersome
class EXPIMP_ACTONQTSO copyFileData_c
{
protected:
    //this can be absolute or relative, file or directory
    //it can also be several paths separated by newline
    QString sourcePath_pro;
    //this can be absolute or relative, file or directory
    //must be compatible with the source:
    //can't copy a folder over a file
    QString destinationPath_pro;
public:
    enum class transferType_ec
    {
        //all the transfers are done by block read/write to be able to cancel/resume easily
        empty = 0
        , copy = 1
        //regular move that every OS uses, aka copy and if success delete the source
        , move = 2

        //TODO? a way to mark which file transfers have been left midway, this might signal the need for a greater thing called
        //"storing execution state (controlled)interruption-proof"

        //DONTDO add an option to truncate while the read is in RAM still (which can be dangerous,
        //but will make this transfer technique use 0 extra space)


        //move by "blocks" starting from the source "end" to its begining,
        //truncating the source on each block moved, in this order, aka first it writes then it truncates (it's safe)
        //it's slow in the destination because it needs to shift the data of all the previous transfered
        //blocks forward for every data block writing, so every block moved does a rewriting of all the previous blocks
        //uses WAY less extra space than the regular move, which temporarily requires 2x"of the moved file size" (during the move)
        //use only when low on space or really fast transfers
        //good for moving big file/s in a SSD low on space?
        //it WILL, when I implement that FUTURE comment above, be possible to fast-"abused"?
        //if storage space is low but ram size is big enough to contain all the data, of a file, in one/few reads
        //this can be done by changing the buffer size
        //"""resumes""" by default basically it will keep shifting w/e was left midway to the point that if destination already exists
        //it will prepend the source to it
        //unlike the destinatioTreatment resume options if the interruption is hard, like power outage or a sigkill, resuming might go wrong
        //becase it can happen before the source is truncated, so the destination gets the "interruped block" appended twice
        , trueMove = 3
        //a variation of the above, instead of writing and shifting previous written data for each block
        //this one writes a file for each block, when all the blocks/files are at the destination it joins them.
        //It's 1 read on source + 1 write on destination + 1 another read and write (for the join)
        //way more optimal than trueMove, but file exploring software might slow/crash when navigating
        //a folder with 1000+ files, using small (<1024k) blocks/bufferSize_pro is not advised
        , trueMoveFiles = 4
    };
    static EXPIMP_ACTONQTSO const QMap<QString, transferType_ec> strToTransferTypeMap_sta_con;
    static EXPIMP_ACTONQTSO const std::unordered_map<transferType_ec, QString> transferTypeToStrUMap_sta_con;

    enum class destinationTreatment_ec
    {
        empty = 0
        //error on any overwrite
        , overwriteIsError = 1
        //checks first if the destination is the same as the source, if not "error" else "ignore-success"
        , overwriteIsErrorOnlyIfDifferent = 2
        //checks first if the source and the destination are the same file to avoid a writing
        //this is 2 x reads at best and 2 x reads + 1 x write at worst, regular overwrite is 1 read 1 write always
        , overwriteOnlyIfDifferent = 3
        //overwrite always
        , overwrite = 4
        //no overwrite always, ignore existing files
        , noOverwrite = 5
        //see resumeType_ec
        , resume = 6
    };

    static EXPIMP_ACTONQTSO const QMap<QString, destinationTreatment_ec> strToDestinationTreatmentMap_sta_con;
    static EXPIMP_ACTONQTSO const std::unordered_map<destinationTreatment_ec, QString> destinationTreatmentToStrUMap_sta_con;

    enum class resumeType_ec
    {
        empty = 0
        //IMPORTANT
        //Fixed by the transfer type, this is true for trueMove and trueMoveFiles right now
        //the other transfer types support the other resume types
        , transferTypeImplementation = 1
        //like overwrite but only if the destination is smaller and writing at the end of the destination,
        //if destination is bigger it will be ignored and warned and the transfer will continue
        //this can have some adverse consequences if
        //source or destination changed after the initial interrupted transfer
        //will only check if the source@size > destination@size,
        //and use the destination size as the starting index for the source
        //to resume the transfer
        //should be able to resume successfully if source/destination haven't changed since the transfer interruption
        //use only? to copy large amounts of data that won't change in either side but can be interrupted
        //fastest resume method if nothing goes wrong (no changes in the source or destination)
        , stupid = 2
        //like the above but compares source and destination and only overwrites
        //when a difference is detected (the first one), i.e. destination already exists so a comparison is done
        //between source and destination, in blocks, and as long as they are equal no writing will happen
        //once a difference is found overwriting happens and no more comparison for that file is done
        //in the rare case where the destination file is the same but larger, it will be truncated from the end
        , lazy = 3
        //FUTURE, same as above, but detect the blocks that are different, comparing blockwise (making it per byte would be overkill)
        //so it only overwrites the blocks that are different and not when the first different block is found
        //, smart = 3
    };

    static EXPIMP_ACTONQTSO const QMap<QString, resumeType_ec> strToResumeTypeMap_sta_con;
    static EXPIMP_ACTONQTSO const std::unordered_map<resumeType_ec, QString> resumeTypeToStrUMap_sta_con;


protected:
    transferType_ec transferType_pro = transferType_ec::copy;
    destinationTreatment_ec destinationTreatment_pro = destinationTreatment_ec::resume;
    resumeType_ec resumeType_pro = resumeType_ec::transferTypeImplementation;

    //applies to files and directories
    bool copyHidden_pro = true;

    //only files
    //applied to the filenames,
    //several regexs apply "or" wise
    QStringList sourceFilenameRegexFilters_pro;
    QStringList sourceFilenameFullExtensions_pro;

    //only for directories
    bool navigateSubdirectories_pro = true;
    bool navigateHidden_pro = true;
    bool copyEmptyDirectories_pro = true;
    //FUTURE directories with no files option?, because copyEmptyDirectories_pri will copy directories with empty directories in them

    //like in create directory
    bool createDestinationAndParents_pro = false;

    //only affects dir sources, if a file copy errors, save the error
    //but wait till the end to report it, because by default errors immediately
    //stop running the action
    bool stopAllCopyOnFileCopyError_pro = true;
    bool noFilesCopiedIsError_pro = false;

    int_fast64_t bufferSize_pro = 1024 * 1024;
    //above stuff is json save-load-able

    //directoryFilter_c* directoryFilterPtr_pro = nullptr;
    //FUTURE sort options? tho it can be "done" using several copy actions and actionFinished check

    //prevent public assignments
    copyFileData_c& operator=(copyFileData_c const &) = default;
    copyFileData_c& operator=(copyFileData_c&) = default;
    copyFileData_c& operator=(copyFileData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    copyFileData_c(copyFileData_c const&) = default;
    copyFileData_c(copyFileData_c&) = default;

    copyFileData_c() = default;
    explicit copyFileData_c(
            const QString& sourcePath_par_con
            , const QString& destinationPath_par_con
            , const transferType_ec transferType_par_con
            , const destinationTreatment_ec destinationTreatment_par_con
            , const resumeType_ec resumeType_par_con = resumeType_ec::transferTypeImplementation
            , const bool copyHidden_par_con = true
            , const QStringList& sourceFilenameRegexFilters_par_con = QStringList()
            , const QStringList& sourceFilenameFullExtensions_par_con = QStringList()
            , const bool nagivateSubdirectories_par_con = true
            , const bool navigateHidden_par_con = true
            , const bool copyEmptyDirectories_par_con = true
            , const bool createDestinationParent_par_con = false
            , const bool noFilesCopiedIsError_par_con = false
            , const bool stopAllCopyOnFileCopyError_par_con = true
            , const int_fast64_t bufferSize_par_con = 1024 * 1024
    );

//    //copies everything except the mutex and pointers
//    copyFileAction_c(const copyFileAction_c& from_par_con);
//    //required to copy from a pointer dereference, does the same the above
//    copyFileAction_c(copyFileAction_c& from_par);
//    //std::moves everything and nulls pointers
//    copyFileAction_c(copyFileAction_c&& from_par) noexcept;

//    //copies everything except mutex and pointers
//    copyFileAction_c& operator=(const copyFileAction_c& from_par_con);
//    //same as above
//    copyFileAction_c& operator=(copyFileAction_c& from_par);
//    //same as move ctor
//    copyFileAction_c& operator=(copyFileAction_c&& from_par) noexcept;

    QString sourcePath_f() const;

    void setSourcePath_f(const QString& sourcePath_par_con);
    QString destinationPath_f() const;

    void setDestinationPath_f(const QString& destinationPath_par_con);
    transferType_ec transferType_f() const;
    void setTransferType_f(const transferType_ec transferType_par_con);
    destinationTreatment_ec destinationTreatment_f() const;
    void setDestinationTreatment_f(const destinationTreatment_ec& destinationTreatment_par_con);
    bool copyHidden_f() const;
    void setCopyHidden_f(const bool copyHidden_par_con);
    QStringList sourceFilenameFullExtensions_f() const;

    void setSourceFilenameFullExtensions_f(const QStringList& filenameFullExtensions_par_con);
    bool navigateSubdirectories_f() const;
    void setNavigateSubdirectories_f(const bool includeSubdirectories_par_con);
    bool navigateHidden_f() const;
    void setNavigateHidden_f(const bool navigateHidden_par_con);
    bool copyEmptyDirectories_f() const;
    void setCopyEmptyDirectories_f(const bool copyEmptyDirectories_par_con);
    QStringList sourceFilenameRegexFilters_f() const;
    void setSourceFilenameRegexFilters_f(const QStringList& sourceFilenameRegexFilters_par_con);
    bool createDestinationAndParents_f() const;
    void setCreateDestinationAndParents_f(const bool createDestinationParent_par_con);
    bool stopAllCopyOnFileCopyError_f() const;
    void setStopAllCopyOnFileCopyError_f(const bool stopAllCopyOnFileCopyError_par_con);
    bool noFilesCopiedIsError_f() const;
    void setNoFilesCopiedIsError_f(const bool noFilesCopiedIsError_par_con);
    int_fast64_t bufferSize_f() const;
    void setBufferSize_f(const int_fast64_t& bufferSize_par_con);
    resumeType_ec resumeType_f() const;
    void setResumeType_f(const resumeType_ec resumeType_par_con);

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;
};

class QFileInfo;

class EXPIMP_ACTONQTSO copyFileAction_c : public action_c, public copyFileData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidAction_f(textCompilation_c* errors_par = nullptr) const override;

    //uint_fast64_t derivedUpdateActionStringIdDependencies_f(const QString& , const QString& ) override;
    //uint_fast64_t derivedActionStringIdDependencyCount_f(const QString& ) const override;
    //uint_fast64_t derivedStringTriggerCreationConflictCount_f(const QString& ) const override;
    uint_fast64_t derivedUpdateStringTriggerDependecies_f(const QString& oldStringTrigger_par_con, const QString& newStringTrigger_par_con) override;
    uint_fast64_t derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const override;
    //QSet<QString> derivedStringTriggerCreationCollection_f() const override;
    QSet<QString> derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const override;

    action_c* derivedClone_f() const override;

    baseActionExecution_c* createExecutionObj_f(actionExecutionResult_c* actionDataExecutionResult_ptr_par) override;
    actionType_ec type_f() const override;
    //QString typeStr_f() const override;

    QString derivedReference_f() const override;

public:
    copyFileAction_c() = default;
    copyFileAction_c(actonDataHub_c* parent_par, const actionData_c& actionData_par_con, const copyFileData_c& copyFile_par_con);

    void updateCopyFileData_f(const copyFileData_c& copyFileData_par_con);

    QString sourcePathParsed_f() const;
    QString destinationPathParsed_f() const;
    QStringList sourceFilenameFullExtensionsParsed_f() const;
    QStringList sourceFilenameRegexFiltersParsed_f() const;

    static filterOptions_s setupFilterOptions_f(const copyFileAction_c* const copyFileDataPtr_par, const QFileInfo& sourceFileInfo_par_con);
    //doesn't filter just setups the directoryFilter_c, use filter or the threaded version (connect the signals prior)
    static directoryFilter_c* testSourceFileListV2_f(const copyFileAction_c* const copyFileDataPtr_par, textCompilation_c* errors_ptr = nullptr);
};

#endif // ACTONQTSO_CREATEDIRECTORY_HPP
