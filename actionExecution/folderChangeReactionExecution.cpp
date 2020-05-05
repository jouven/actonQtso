#include "folderChangeReactionExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actions/folderChangeReaction.hpp"
#include "../actonDataHub.hpp"
#include "../actions/runProcess.hpp"
#include "../actions/deleteFileDir.hpp"
#include "../actions/copyFile.hpp"

#include "cryptoQtso/hashQt.hpp"
#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "filterDirectoryQtso/filterDirectory.hpp"
#include "textQtso/text.hpp"

#include <QFileInfo>
#include <QTimer>
#include <QCollator>

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

int_fast64_t fileState_c::currentFileSize_f() const
{
    return currentFileSize_pri;
}

int_fast64_t fileState_c::currentLastModificationDatetimeMs_f() const
{
    return currentLastModificationDatetimeMs_pri;
}

uint_fast64_t fileState_c::currentHash_f() const
{
    return currentHash_pri;
}

bool fileState_c::exists_f() const
{
    return exists_pri;
}

uint_fast64_t fileState_c::oldFileSize_f() const
{
    return oldFileSize_pri;
}

int_fast64_t fileState_c::oldLastModificationDatetimeMs_f() const
{
    return oldLastModificationDatetimeMs_pri;
}

uint_fast64_t fileState_c::oldHash_f() const
{
    return oldHash_pri;
}

bool fileState_c::oldExists_f() const
{
    return oldExists_pri;
}

QString fileState_c::filePath_f() const
{
    return filePath_pri;
}

bool fileState_c::sizeChanged_f() const
{
    return oldIinitalized_pri and (currentFileSize_pri not_eq oldFileSize_pri);
}

bool fileState_c::sizeIncreased_f() const
{
    return oldIinitalized_pri and (oldFileSize_pri < currentFileSize_pri);
}

bool fileState_c::sizeDecreased_f() const
{
    return oldIinitalized_pri and (oldFileSize_pri > currentFileSize_pri);
}

bool fileState_c::lastModificationDatetimeChanged_f() const
{
    return oldIinitalized_pri and (oldLastModificationDatetimeMs_pri not_eq currentLastModificationDatetimeMs_pri);
}

bool fileState_c::isNew_f() const
{
    return not oldIinitalized_pri and exists_pri;
}

bool fileState_c::isDeleted_f() const
{
    return oldExists_pri and not exists_pri;
}

bool fileState_c::contentChanged_f() const
{
    bool contentChangedResultTmp(false);
    while (oldIinitalized_pri)
    {
        if (sizeChanged_f())
        {
            contentChangedResultTmp = true;
            break;
        }

        if (currentHashSet_pri)
        {
            contentChangedResultTmp = currentHash_pri not_eq oldHash_pri;
            break;
        }

        break;
    }
    return contentChangedResultTmp;
}

void fileState_c::updateCurrentValues_f()
{
    QFileInfo fileTmp(filePath_pri);
    if (fileTmp.exists())
    {
        exists_pri = true;
        if (fileTmp.isFile())
        {
            currentFileSize_pri = fileTmp.size();
            currentLastModificationDatetimeMs_pri = fileTmp.lastModified().toMSecsSinceEpoch();
            if (requiresHash_pri)
            {
                hasher_c hasherTmp(
                            hasher_c::inputType_ec::file
                            , filePath_pri
                            , hasher_c::outputType_ec::unsignedXbitInteger
                            , hasher_c::hashType_ec::XXHASH64
                );
                hasherTmp.generateHash_f();
                currentHash_pri = hasherTmp.hash64BitNumberResult_f();
                currentHashSet_pri = true;
            }
        }
        else
        {
            //ignore folders
        }
    }
    else
    {
        exists_pri = false;
    }
}

void fileState_c::copyCurrentToOld_f()
{
    oldHash_pri = currentHash_pri;
    oldHashSet_pri = currentHashSet_pri;
    oldExists_pri = exists_pri;
    oldFileSize_pri = currentFileSize_pri;
    oldLastModificationDatetimeMs_pri = currentLastModificationDatetimeMs_pri;
    oldIinitalized_pri = true;
}

fileState_c::fileState_c(
        const QString& filePath_par_con
        , const bool requiresHash_par_con
        )
    : filePath_pri(filePath_par_con)
    , requiresHash_pri(requiresHash_par_con)
    , initialized_pri(true)
{
    updateCurrentValues_f();
}

void fileState_c::updateFileValues_f()
{
    copyCurrentToOld_f();
    updateCurrentValues_f();
#ifdef DEBUGJOUVEN
    qDebug() << "filePath_pri " << filePath_pri << endl
             << "currentFileSize_pri " << currentFileSize_pri << endl
             << "exists_pri " << exists_pri << endl
             << "oldExists_pri " << oldExists_pri << endl
             << "oldIinitalized_pri " << oldIinitalized_pri << endl;
#endif
}

folderChangeReactionActionExecution_c::folderChangeReactionActionExecution_c(
        actionDataExecutionResult_c* actionExecutionResultObj_par_con
        , folderChangeReactionAction_c* folderChangeReactionActionPtr_par
)
    : baseActionExecution_c(actionExecutionResultObj_par_con)
    , folderChangeReactionActionPtr_pri(folderChangeReactionActionPtr_par)
{}

bool folderChangeReactionActionExecution_c::anyChangeToReact_f(const fileState_c& fileStateObj_par_con) const
{
    bool resultTmp(false);
    for (const folderChangeReactionData_c::changeType_ec changeType_ite_con : folderChangeReactionActionPtr_pri->changesToMonitor_f())
    {
        if (resultTmp)
        {
            break;
        }

        if (changeType_ite_con == folderChangeReactionData_c::changeType_ec::fileModifiedDateTimeChanged)
        {
            resultTmp = fileStateObj_par_con.lastModificationDatetimeChanged_f();
            continue;
        }

        //without previous state saving, first cycle can't be used to determine if a file was created/removed
        //the other ifs are fine because they check if there is a previous state
        if (not firstCycle_pri)
        {
            if (changeType_ite_con == folderChangeReactionData_c::changeType_ec::fileCreated)
            {
                resultTmp = fileStateObj_par_con.isNew_f();
#ifdef DEBUGJOUVEN
                //qDebug() << "fileStateObj_par_con.isNew_f() " << resultTmp << endl;
#endif
                continue;
            }

            if (changeType_ite_con == folderChangeReactionData_c::changeType_ec::fileRemoved)
            {
                resultTmp = fileStateObj_par_con.isDeleted_f();
                continue;
            }
        }

        if (changeType_ite_con == folderChangeReactionData_c::changeType_ec::fileSizeChanged)
        {
            //check types of size changes

            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::any)
            {
                resultTmp = fileStateObj_par_con.sizeChanged_f();
                continue;
            }

            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::anyDecrease)
            {
                resultTmp = fileStateObj_par_con.sizeDecreased_f();
                continue;
            }

            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::anyIncrease)
            {
                resultTmp = fileStateObj_par_con.sizeIncreased_f();
                continue;
            }

            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::fixedValue)
            {
                resultTmp = fileStateObj_par_con.currentFileSize_f() == folderChangeReactionActionPtr_pri->fileSizeChangeFixedValue_f();
                continue;
            }

            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::threshold)
            {
                if (folderChangeReactionActionPtr_pri->fileSizeChangeThreshold_f() >= 0
                    and fileStateObj_par_con.currentFileSize_f() > folderChangeReactionActionPtr_pri->fileSizeChangeThreshold_f())
                {
                    resultTmp = true;
                    continue;
                }

                if (folderChangeReactionActionPtr_pri->fileSizeChangeThreshold_f() < 0
                    and fileStateObj_par_con.currentFileSize_f() < (folderChangeReactionActionPtr_pri->fileSizeChangeThreshold_f() * -1))
                {
                    resultTmp = true;
                    continue;
                }
            }
        }

        if (changeType_ite_con == folderChangeReactionData_c::changeType_ec::fileContentsChanged)
        {
            resultTmp = fileStateObj_par_con.contentChanged_f();
            continue;
        }
    }
#ifdef DEBUGJOUVEN
    //qDebug() << "any change to react resultTmp " << resultTmp << endl;
#endif
    return resultTmp;
}

action_c* folderChangeReactionActionExecution_c::getActionForReaction_f(const QString& filePath_par_con)
{
    action_c* actionResultTmp(nullptr);
    action_c* clonedActionTmp(nullptr);
    action_c* originalActionTmp(nullptr);
    bool errorHappenedTmp(false);
    bool reactionTypeActionTypeMismatchTmp(false);
    bool actionWasEnabledTmp(false);
    bool actionNotFoundTmp(false);
    //all the reaction types "execution types" make a new thread when execute
    while (true)
    {
        //fetch the action using the actionStringId
        int_fast64_t reactionActionIdTmp(actonDataHub_ptr_ext->actionDataStringIdToActionDataId_f(folderChangeReactionActionPtr_pri->reactionActionStringId_f()));

        {
#ifdef DEBUGJOUVEN
            //qDebug() << "reactionActionIdTmp " << reactionActionIdTmp << endl;
#endif
            originalActionTmp = actonDataHub_ptr_ext->action_ptr_f(reactionActionIdTmp);
            if (originalActionTmp == nullptr)
            {
                actionNotFoundTmp = true;
                errorHappenedTmp = true;
            }
            else
            {
                if (originalActionTmp->enabled_f())
                {
                    actionWasEnabledTmp = true;
                    errorHappenedTmp = true;
                }
                if (not folderChangeReactionData_c::isValidActionForReaction_f(originalActionTmp, folderChangeReactionActionPtr_pri->reactionType_f()))
                {
                    reactionTypeActionTypeMismatchTmp = true;
                    errorHappenedTmp = true;
                }
            }
            if (errorHappenedTmp)
            {
                break;
            }
        }

        //clone
        clonedActionTmp = actonDataHub_ptr_ext->action_ptr_f(reactionActionIdTmp)->clone_f();
        //enable the cloned action
        clonedActionTmp->setEnabled_f(true);
        //change parent of the clonced action
        clonedActionTmp->setParent(this);

        if (folderChangeReactionActionPtr_pri->reactionType_f() == folderChangeReactionData_c::reactionType_ec::execute)
        {
            runProcessAction_c* runProcessPtrTmp(static_cast<runProcessAction_c*>(clonedActionTmp));
            //modify execute path
            runProcessPtrTmp->setProcessPath_f(filePath_par_con);
            break;
        }

        if (folderChangeReactionActionPtr_pri->reactionType_f() == folderChangeReactionData_c::reactionType_ec::runProcess)
        {
            runProcessAction_c* runProcessPtrTmp(static_cast<runProcessAction_c*>(clonedActionTmp));
            //optional replace an argument/s
            if (not folderChangeReactionActionPtr_pri->argumentPlaceHolderToReplaceWithChangedFilePathParsed_f().isEmpty())
            {
                for (argument_c& argument_ite : runProcessPtrTmp->arguments_f())
                {
                    if (argument_ite.argumentParsed_f().contains(folderChangeReactionActionPtr_pri->argumentPlaceHolderToReplaceWithChangedFilePathParsed_f()))
                    {
                        QString argumentToReplaceTmp(argument_ite.argumentParsed_f());
                        argumentToReplaceTmp.replace(folderChangeReactionActionPtr_pri->argumentPlaceHolderToReplaceWithChangedFilePathParsed_f(), filePath_par_con);
                        argument_ite.setArgument_f(argumentToReplaceTmp);
                    }
                }
            }
            break;
        }

        if (folderChangeReactionActionPtr_pri->reactionType_f() == folderChangeReactionData_c::reactionType_ec::remove)
        {
            deleteFileDirAction_c* deleteFileDirPtrTmp(static_cast<deleteFileDirAction_c*>(clonedActionTmp));
            //modify delete path
            deleteFileDirPtrTmp->setPath_f(filePath_par_con);
            break;
        }

        if (folderChangeReactionActionPtr_pri->reactionType_f() == folderChangeReactionData_c::reactionType_ec::copyMove)
        {
            copyFileAction_c* copyFilePtrTmp(static_cast<copyFileAction_c*>(clonedActionTmp));
            //modify source path
            copyFilePtrTmp->setSourcePath_f(filePath_par_con);
            break;
        }

        break;
    }

    if (errorHappenedTmp)
    {
        if (actionNotFoundTmp)
        {
            Q_EMIT addError_signal(
            {
                            "Action with stringId {0} not found"
                            , folderChangeReactionActionPtr_pri->reactionActionStringId_f()
            });
        }
        if (actionWasEnabledTmp)
        {
            Q_EMIT addError_signal(
            {
                            "Action stringId {0} was enabled"
                            , originalActionTmp->stringId_f()
            });
        }
        if (reactionTypeActionTypeMismatchTmp)
        {
            Q_EMIT addError_signal(
            {
                            "Action stringId {0}, wrong action type {1} for this reaction type {2}"
                            , originalActionTmp->stringId_f()
                            , originalActionTmp->typeStr_f()
                            , folderChangeReactionData_c::reactionTypeToStrUMap_sta_con.at(folderChangeReactionActionPtr_pri->reactionType_f())
            });
        }

        //cloned action is deleted in monitoringCheckReactionPhaseEnded_f
//        //delete the cloned action
//        actionTmp->deleteLater();
    }
    else
    {
        //validate the modified action
        textCompilation_c errors2Tmp;
        if (clonedActionTmp->isFieldsActionValid_f(std::addressof(errors2Tmp)))
        {
            actionResultTmp = clonedActionTmp;
        }
        else
        {
            textCompilation_c errors1Tmp;
            text_c errorTmp("Error/s found in action stringId \"{0}\"", clonedActionTmp->stringId_f());
            errors1Tmp.append_f(errorTmp);
            errors1Tmp.append_f(errors2Tmp);
            Q_EMIT addErrors_signal(errors1Tmp);
        }
    }

    return actionResultTmp;
}

void folderChangeReactionActionExecution_c::executeReaction_f(
        const QString& monitoredFile_par_con
        , const bool sequentialReaction_par_con)
{
    fileState_c fileStateTmp(monitoredFiles_pri.value(monitoredFile_par_con));
    if (anyChangeToReact_f(fileStateTmp))
    {

#ifdef DEBUGJOUVEN
        qDebug() << "afer anyChangeToReact_f" << endl;
#endif
        action_c* clonedActionTmp(getActionForReaction_f(monitoredFile_par_con));
        if (clonedActionTmp not_eq nullptr)
        {
            actionDataExecutionResult_c* clonedActionDataExecutionResultTmp(clonedActionTmp->createGetActionDataExecutionResult_ptr_f());
            QObject::connect(
                        clonedActionDataExecutionResultTmp
                        , &actionDataExecutionResult_c::finished_signal
                        , this
                        , &folderChangeReactionActionExecution_c::monitoringCheckReactionPhaseEnded_f
                        //            , Qt::UniqueConnection
                        );
            if (sequentialReaction_par_con and not monitoredFilesSorted_pri.empty())
            {
                QObject::connect(
                            clonedActionDataExecutionResultTmp
                            , &actionDataExecutionResult_c::finished_signal
                            , this
                            , &folderChangeReactionActionExecution_c::monitoringReact_f
                            //            , Qt::UniqueConnection
                            );
            }
            actionDataExecutionResult_c* actionDataExecutionResultTmp(folderChangeReactionActionPtr_pri->actionDataExecutionResult_ptr_f());
            //"link" the error/s signals of the reaction actions to this action error/s signal
            QObject::connect(
                        clonedActionDataExecutionResultTmp
                        , &actionDataExecutionResult_c::error_signal
                        , actionDataExecutionResultTmp
                        , &actionDataExecutionResult_c::error_signal);
            QObject::connect(
                        clonedActionDataExecutionResultTmp
                        , &actionDataExecutionResult_c::errors_signal
                        , actionDataExecutionResultTmp
                        , &actionDataExecutionResult_c::errors_signal);
            executingReactionActions_pri.emplace_back(clonedActionTmp);
            clonedActionTmp->tryExecute_f();
            //TODO action results class, serialize results so they can be logged, since the reaction action results can't be read
            //because the action is created and destroyed
        }
        else
        {
            //getActionForReaction_f already signals errors if no action can be fectched for the reaction
        }
    }
    else
    {
        monitoringCheckReactionPhaseEnded_f(nullptr);
    }
}

void folderChangeReactionActionExecution_c::monitoringReact_f()
{
    while (true)
    {
        if (folderChangeReactionActionPtr_pri->isStoppingExecution_f())
        {
            Q_EMIT anyFinish_signal();
            break;
        }

        //the monitored folder is empty from the start or has been for the previous monitoring cycle and the current one
        if (monitoredFilesSorted_pri.empty())
        {
            monitoringScheduler_f();
            break;
        }

        if (folderChangeReactionActionPtr_pri->sequentialReaction_f())
        {
            QString monitoredFileTmp(monitoredFilesSorted_pri.front());
            monitoredFilesSorted_pri.erase(monitoredFilesSorted_pri.begin());

            executeReaction_f(monitoredFileTmp, folderChangeReactionActionPtr_pri->sequentialReaction_f());
        }
        else
        {
            //loop the files
            for (const QString& monitoredFileStr_ite_con : monitoredFilesSorted_pri)
            {
                executeReaction_f(monitoredFileStr_ite_con, folderChangeReactionActionPtr_pri->sequentialReaction_f());
            }
        }
        break;
    }
}

void folderChangeReactionActionExecution_c::monitoringCycleEnd_f()
{
    //remove deleted files from monitoring
    for (const QString& itemToRemove_ite_con : monitoredFilesToRemove_pri)
    {
        monitoredFiles_pri.remove(itemToRemove_ite_con);
    }
    monitoredFilesToRemove_pri.clear();
    monitoringScheduler_f();
}

void folderChangeReactionActionExecution_c::monitoringCheckReactionPhaseEnded_f(action_c* action_par)
{
    if (action_par not_eq nullptr)
    {
        auto findResultTmp(std::find(executingReactionActions_pri.begin(), executingReactionActions_pri.end(), action_par));
        if (findResultTmp not_eq executingReactionActions_pri.end())
        {
            executingReactionActions_pri.erase(findResultTmp);
            //delete the cloned action
            (*findResultTmp)->deleteLater();
        }
    }
    if (executingReactionActions_pri.empty())
    {
        if (folderChangeReactionActionPtr_pri->isStoppingExecution_f())
        {
            Q_EMIT anyFinish_signal();
        }
        else
        {
            monitoringCycleEnd_f();
        }
    }
}

void folderChangeReactionActionExecution_c::monitoringGatherFiles_f()
{
    //FUTURE --> 0 read from the state file
    //1 do an initial read of the folder and fill the new fields
    //2 start the loop, wait the interval, update the old fields values with the previous new fields,
    //update the new fields with the current file values and finally compare to check the monitored changes

    if (directoryFilter_pri == nullptr)
    {
        filterOptions_s filterOptionsTmp;
        filterOptionsTmp.navigateSubdirectories_pub = folderChangeReactionActionPtr_pri->recurseFolders_f();
        filterOptionsTmp.useAbsolutePaths_pub = false;
        filterOptionsTmp.listFiles_pub = true;
        filterOptionsTmp.listHidden_pub = true;
        filterOptionsTmp.navigateHiddenDirectories_pub = true;
        filterOptionsTmp.listDirectories_pub = false;
        filterOptionsTmp.listEmptyDirectories_pub = false;

        directoryFilter_pri = new directoryFilter_c(folderChangeReactionActionPtr_pri->folderPathParsed_f(), filterOptionsTmp);
        QObject::connect(this, &folderChangeReactionActionExecution_c::destroyed, directoryFilter_pri, &directoryFilter_c::deleteLater);
    }
    std::vector<QString> fileListResultTmp(directoryFilter_pri->filter_f());
#ifdef DEBUGJOUVEN
    qDebug() << "fileListResultTmp.size() " << fileListResultTmp.size() << endl;
#endif

    //update existing files details
    QMap<QString, fileState_c>::iterator iteTmp(monitoredFiles_pri.begin());
    while (iteTmp not_eq monitoredFiles_pri.end())
    {
        iteTmp.value().updateFileValues_f();
        if (not iteTmp.value().exists_f())
        {
            monitoredFilesToRemove_pri.emplace_back(iteTmp.key());
        }
        ++iteTmp;
    }

    //insert new files
    for (const QString& filePath_ite_con : fileListResultTmp)
    {
        if (monitoredFiles_pri.contains(filePath_ite_con))
        {
            //nothing
        }
        else
        {
            monitoredFiles_pri.insert(filePath_ite_con, fileState_c(filePath_ite_con, hashRequired_pri));
        }
    }

    std::vector<QString> monitoredFilesSortedTmp(monitoredFiles_pri.keys().toVector().toStdVector());
    if (not monitoredFiles_pri.empty())
    {
        //sort files by reactionOrder
        QCollator collatorTmp;
        collatorTmp.setNumericMode(true);

        std::sort(monitoredFilesSortedTmp.begin(), monitoredFilesSortedTmp.end(),
                  [this, &collatorTmp](const fileState_c& a_par_con, const fileState_c& b_par_con) -> bool
        {
            bool resultTmp(false);
            while (true)
            {
                if (folderChangeReactionActionPtr_pri->reactionOrder_f() == folderChangeReactionData_c::reactionOrder_ec::newestFirst)
                {
                    resultTmp = a_par_con.currentLastModificationDatetimeMs_f() > b_par_con.currentLastModificationDatetimeMs_f();
                    break;
                }

                if (folderChangeReactionActionPtr_pri->reactionOrder_f() == folderChangeReactionData_c::reactionOrder_ec::alphabetical)
                {
                    resultTmp = collatorTmp.compare(a_par_con.filePath_f(), b_par_con.filePath_f()) < 0;
                    break;
                }

                if (folderChangeReactionActionPtr_pri->reactionOrder_f() == folderChangeReactionData_c::reactionOrder_ec::smallerFirst)
                {
                    resultTmp = a_par_con.currentFileSize_f() < b_par_con.currentFileSize_f();
                    break;
                }

                break;
            }
            return resultTmp;
        });
    }

    if (folderChangeReactionActionPtr_pri->reverseReactionOrder_f())
    {
        std::reverse(monitoredFilesSortedTmp.begin(), monitoredFilesSortedTmp.end());
    }

    monitoredFilesSorted_pri = monitoredFilesSortedTmp;

    Q_EMIT monitoringGatherFilesFinished_signal();
}

void folderChangeReactionActionExecution_c::launchMonitoringGatherFilesThread_f()
{
    threadedFunction_c* threadedFunction_ptr(new threadedFunction_c(std::bind(&folderChangeReactionActionExecution_c::monitoringGatherFiles_f, this), true));
    QObject::connect(threadedFunction_ptr, &threadedFunction_c::finished, threadedFunction_ptr, &threadedFunction_c::deleteLater);
    QObject::connect(this, &folderChangeReactionActionExecution_c::monitoringGatherFilesFinished_signal, threadedFunction_ptr, &threadedFunction_c::quit);
    threadedFunction_ptr->start();
}

void folderChangeReactionActionExecution_c::monitoringScheduler_f()
{
    uint_fast64_t currentDateTimeTmp(QDateTime::currentMSecsSinceEpoch());
    if (lastMonitoringStart_pri == 0)
    {
        lastMonitoringStart_pri = currentDateTimeTmp;
        launchMonitoringGatherFilesThread_f();
    }
    else
    {
        firstCycle_pri = false;
        uint_fast64_t timeDifferenceMsTmp(currentDateTimeTmp - lastMonitoringStart_pri);
        //if the monitor interval time is bigger than the time passed since the previous monitoring cycle start
        //wait and then come back to the monitoring scheduler
        if (folderChangeReactionActionPtr_pri->monitorIntervalMs_f() > timeDifferenceMsTmp)
        {
            QTimer::singleShot(timeDifferenceMsTmp, this, &folderChangeReactionActionExecution_c::monitoringScheduler_f);
        }
        else
        {
            lastMonitoringStart_pri = currentDateTimeTmp;
            launchMonitoringGatherFilesThread_f();
        }
    }
}

void folderChangeReactionActionExecution_c::derivedExecute_f()
{
    hashRequired_pri = folderChangeReactionActionPtr_pri->changesToMonitor_f().count(folderChangeReactionData_c::changeType_ec::fileContentsChanged) == 1;

    QObject::connect(
                this
                , &folderChangeReactionActionExecution_c::monitoringGatherFilesFinished_signal
                , this
                , &folderChangeReactionActionExecution_c::monitoringReact_f);
    QTimer::singleShot(0, this, &folderChangeReactionActionExecution_c::monitoringScheduler_f);
}

void folderChangeReactionActionExecution_c::derivedStop_f()
{
    if (directoryFilter_pri not_eq nullptr)
    {
        directoryFilter_pri->stopFiltering_f();
    }
    for (action_c* const actionPtr_ite_con : executingReactionActions_pri)
    {
        actionPtr_ite_con->tryStopExecution_f(folderChangeReactionActionPtr_pri->killingExecutionAfterTimeout_f());
    }
}

void folderChangeReactionActionExecution_c::derivedKill_f()
{
    //not required
    //it can always be stopped (it's not like runProcess)
}



