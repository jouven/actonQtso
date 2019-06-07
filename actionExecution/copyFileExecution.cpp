#include "copyFileExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actions/copyFile.hpp"
#include "../reused/sameFileAlgo.hpp"
#include "../actonDataHub.hpp"

#include "../essentialQtso/macros.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

copyFileActionExecution_c::copyFileActionExecution_c(
        actionDataExecutionResult_c* actionExecutionResultObj_par_con
        , copyFileAction_c* copyFileActionPtr_par
)
    : baseActionExecution_c(actionExecutionResultObj_par_con)
    , copyFileActionPtr_pri(copyFileActionPtr_par)
{}

bool copyFileActionExecution_c::sameFile_f(
        const QString& source_par_con
        , const QString& destination_par_con
)
{
    checkSameFile_c checkSameFileTmp(source_par_con, destination_par_con);
    checkSameFile_ptr = std::addressof(checkSameFileTmp);
    bool resultTmp(checkSameFileTmp.checkIfSameFile_f());
    if (resultTmp)
    {
        //good
    }
    else
    {
        diffPos_pri = checkSameFileTmp.indexWhereFilesAreDifferent_f();
    }
    {
        QMutexLocker mutexLockerTmp(std::addressof(checkSameFileMutex_pri));
        checkSameFile_ptr = nullptr;
    }
    return resultTmp;
}

//use only for trueMove
//source file must exist, destination may or may not exist
void copyFileActionExecution_c::blockMove_f(
        const QString& source_par_con
        , const QString& destination_par_con
        , QString* errorStrPtr_par
)
{
    //move by blocks, resumes by default only if the previous transfer used this method, otherwise it will go wrong
    while (true)
    {
        QFile sourceFileTmp(source_par_con);
        int_fast64_t sourceCurrentFileSizeTmp(sourceFileTmp.size());
        if (sourceFileTmp.open(QIODevice::ReadWrite))
        {
            //good
        }
        else
        {
            APPENDSTRPTR(errorStrPtr_par, "Error when opening source file: " + source_par_con)
            break;
        }

        QFile destinationFileTmp(destination_par_con);
        if (destinationFileTmp.open(QIODevice::ReadWrite))
        {
            //0 size case
            if (sourceCurrentFileSizeTmp == 0)
            {
                break;
            }
        }
        else
        {
            APPENDSTRPTR(errorStrPtr_par, "Error when opening destination file: " + destination_par_con)
            break;
        }

        //would use size_t but qt uses qint64 making it unsigned vs signed, compiler complains
        qint64 bytesReadSourceTmp(0);
        qint64 bytesWrittenDestinationTmp(0);
        qint64 bufferSizeTmp(copyFileActionPtr_pri->bufferSize_f());
        //remove?
//        if (bufferSizeTmp > sourceCurrentFileSizeTmp)
//        {
//            bufferSizeTmp = sourceCurrentFileSizeTmp;
//        }

        while (true)
        {
            if (pleaseStop_pri)
            {
                break;
            }

            //check file size, if > 0, shift everything in the destination
            int_fast64_t destinationFileSizeTmp(destinationFileTmp.size());
            if (destinationFileSizeTmp > 0)
            {
                int_fast64_t destinationBufferSizeTmp(copyFileActionPtr_pri->bufferSize_f());
                //if the source file size is smaller than the destination buffer
                //adjust the buffer because otherwise it would shift more space than it would need
                int_fast64_t resizeSizeTmp(destinationBufferSizeTmp);
                if (resizeSizeTmp > sourceCurrentFileSizeTmp)
                {
                    resizeSizeTmp = sourceCurrentFileSizeTmp;
                }

                //resize the destination to make space for what will come, which is aditional space
                //to allow the shifting and after, the new data that will be at the begining
                int_fast64_t destinationNewFileSizeTmp(destinationFileTmp.size() + resizeSizeTmp);
                destinationFileTmp.resize(destinationNewFileSizeTmp);

                //recalculate the destination buffer size according to the OLD size
                if (destinationBufferSizeTmp > destinationFileSizeTmp)
                {
                    destinationBufferSizeTmp = destinationFileSizeTmp;
                }

                //get the READ seek position back 1 buffer from the OLD end
                int_fast64_t seekReadTmp(destinationFileSizeTmp - destinationBufferSizeTmp);
                //I should  comment this because I lost a full evening on this
                //and it's a obvious after the fact but it won't be after weeks/months/years.
                //When shifting the file to make space for the new data that I'll come from the source
                //the file is resized to it's current size + the block size and all the data from the previous
                //size-index it's read in blocks, from the end of the file to the begining of the file
                //but where is written? (this is what took me "some time") it's written one block size before
                //the new-current end, in the same way as the reads,
                //from end to begining plus no worries about going negative index wise
                //since the write index is one buffer ahead and the read index will go negative first
                int_fast64_t seekWriteTmp(destinationFileTmp.size() - destinationBufferSizeTmp);
                while (true)
                {
                    std::string destinationBufferTmp(destinationBufferSizeTmp, '\0');
                    //seek before the end
                    if (destinationFileTmp.seek(seekReadTmp))
                    {
                        //good
                    }
                    else
                    {
                        APPENDSTRPTR(errorStrPtr_par, "Error when seeking destination file: " + destination_par_con)
                        break;
                    }
                    //read to the end
                    int_fast64_t destinationReadSizeTmp(destinationFileTmp.read(&destinationBufferTmp[0], destinationBufferSizeTmp));
//#ifdef DEBUGJOUVEN
//                    qDebug() << "destinationBufferTmp " << QString::fromStdString(destinationBufferTmp);
//                    qDebug() << "destinationBufferSizeTmp " << QString::number(destinationBufferSizeTmp);
//                    qDebug() << "seekReadTmp " << QString::number(seekReadTmp);
//                    qDebug() << "seekWriteTmp " << QString::number(seekWriteTmp);
//#endif
                    if (destinationReadSizeTmp == -1
                        or destinationReadSizeTmp not_eq destinationBufferSizeTmp)
                    {
                        APPENDSTRPTR(errorStrPtr_par, "Error when reading destination file: " + destination_par_con)
                        break;
                    }
                    else
                    {
                        //good
                    }
                    //seek to the write position, which is 1 buffer forward from the read seek position
                    if (destinationFileTmp.seek(seekWriteTmp))
                    {
                        //good
                    }
                    else
                    {
                        APPENDSTRPTR(errorStrPtr_par, "Error when seeking destination file: " + destination_par_con)
                        break;
                    }
                    //write
                    int_fast64_t destinationWrittenSizeTmp(destinationFileTmp.write(&destinationBufferTmp[0], destinationBufferSizeTmp));
                    if (destinationWrittenSizeTmp == -1
                        or destinationWrittenSizeTmp not_eq destinationBufferSizeTmp
                        or destinationWrittenSizeTmp not_eq destinationReadSizeTmp)
                    {
                        APPENDSTRPTR(errorStrPtr_par, "Error when writing destination file: " + destination_par_con)
                        break;
                    }
                    else
                    {
                        //good
                    }

                    //if seekReadTmp == 0 the entire file has been shifted
                    if (seekReadTmp == 0)
                    {
                        break;
                    }

                    //keep going back
                    //minding not going negative
                    if (destinationBufferSizeTmp > seekReadTmp)
                    {
                        destinationBufferSizeTmp = seekReadTmp;
                        seekReadTmp = 0;
                    }
                    else
                    {
                        seekReadTmp = seekReadTmp - destinationBufferSizeTmp;
                    }
                    seekWriteTmp = seekWriteTmp - destinationBufferSizeTmp;
                }

                //seek destination to the start where the source data block will transfered
                destinationFileTmp.seek(0);
            }

            //calculate 1 buffer block before the end of the source file to start the "first" read
            //minding not to seek into a negative index.
            //If the buffer is bigger than the source file size
            //use the file size as a buffer size
            if (bufferSizeTmp > sourceCurrentFileSizeTmp)
            {
                bufferSizeTmp = sourceCurrentFileSizeTmp;
                sourceCurrentFileSizeTmp = 0;
            }
            else
            {
                sourceCurrentFileSizeTmp = sourceCurrentFileSizeTmp - bufferSizeTmp;
            }

            //seek to that point that was just calculated on the last if-else in the source file to start the first read
            if (sourceFileTmp.seek(sourceCurrentFileSizeTmp))
            {
                //good
            }
            else
            {
                APPENDSTRPTR(errorStrPtr_par, "Error seeking source file: " + source_par_con)
                break;
            }
            std::string copyBufferTmp(bufferSizeTmp, '\0');

            //read at that seeking index
            bytesReadSourceTmp = sourceFileTmp.read(&copyBufferTmp[0], bufferSizeTmp);
//#ifdef DEBUGJOUVEN
//            qDebug() << "copyBufferTmp " << QString::fromStdString(copyBufferTmp);
//            qDebug() << "bufferSizeTmp " << QString::number(bufferSizeTmp);
//#endif
            if (bytesReadSourceTmp == -1
                or bytesReadSourceTmp not_eq bufferSizeTmp)
            {
                APPENDSTRPTR(errorStrPtr_par, "Error when reading source file: " + source_par_con)
                break;
            }
            else
            {
                //good
            }
            //write at (the beginning of) the destination
            bytesWrittenDestinationTmp = destinationFileTmp.write(&copyBufferTmp[0], bufferSizeTmp);
            if (bytesWrittenDestinationTmp == -1
                or bytesWrittenDestinationTmp not_eq bufferSizeTmp
                or bytesReadSourceTmp not_eq bytesWrittenDestinationTmp)
            {
                APPENDSTRPTR(errorStrPtr_par, "Error when writing destination file: " + destination_par_con)
                break;
            }
            else
            {
                //good
            }

//            //check the buffer size
//            if (sourceFileTmp.bytesAvailable() < bufferSizeTmp)
//            {
//                bufferSizeTmp = sourceFileTmp.bytesAvailable();
//            }
            //if the last calculated source size is 0, break, doesn't matter if the (last) resize
            //is done or not because the source will be deleted after
            if (sourceCurrentFileSizeTmp == 0)
            {
                break;
            }
            //resize-shrink the source
            if (sourceFileTmp.resize(sourceCurrentFileSizeTmp))
            {
                //good
            }
            else
            {
                APPENDSTRPTR(errorStrPtr_par, "Error resizing source file: " + source_par_con)
                break;
            }
        }
        break;
    }
}

//source file must exist, destination may or may not exist
void copyFileActionExecution_c::blockCopy_f(
        const QString& source_par_con
        , const QString& destination_par_con
        , QString* errorStrPtr_par
)
{
    //MACRO_ADDACTONQTSOLOG("Block copy source: " + source_par_con + " to " + destination_par_con, logItem_c::type_ec::debug);
    //"resuming" is just starting to read the source from the destination "size index"
    //and writing at the end of the destination file
    //destination is openened by appending:
    //if doesn't exist it creates it
    //if exists writes appending at the end
    while (true)
    {
        QFile sourceFileTmp(source_par_con);
        int_fast64_t sourceCurrentFileSizeTmp(sourceFileTmp.size());
        if (sourceFileTmp.open(QIODevice::ReadOnly))
        {
            //good
        }
        else
        {
            APPENDSTRPTR(errorStrPtr_par, "Error when opening source file: " + source_par_con)
            break;
        }

        QFile destinationFileTmp(destination_par_con);
        bool isResumeTmp(equalOnce_ft(
                             copyFileActionPtr_pri->destinationTreatment_f()
                             , copyFileAction_c::destinationTreatment_ec::tryStupidResume
                             , copyFileAction_c::destinationTreatment_ec::lazyResume)
        );
        if (isResumeTmp ?
            destinationFileTmp.open(QIODevice::Append)
            : destinationFileTmp.open(QIODevice::WriteOnly))
        {
            //if source is 0 size, just create the destination file and break
            if (sourceCurrentFileSizeTmp == 0)
            {
                //truncate the destination if necessary
                if (destinationFileTmp.size() > 0)
                {
                    destinationFileTmp.resize(0);
                }
                break;
            }
        }
        else
        {
            APPENDSTRPTR(errorStrPtr_par, "Error when opening/creating destination file: " + destination_par_con)
            break;
        }

        bool breakAfterTmp(false);
        while (isResumeTmp)
        {
            if (copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::tryStupidResume)
            {
                if (destinationFileTmp.size() == sourceFileTmp.size())
                {
                    //same size, treat as they are the same
                    //tho the rare case where source and destination are the same size but different in content still can happen
                    breakAfterTmp = true;
                    break;
                }

                if (destinationFileTmp.size() > sourceFileTmp.size())
                {
                    //ignore file but warn
                    Q_EMIT addOutput_signal("Destination file" + destination_par_con + " is bigger than source " + source_par_con + ", destination size: "
                                            + QString::number(destinationFileTmp.size()) + ", source size: " + QString::number(sourceFileTmp.size()));
                    //             + " source size: "  + QString::number(sourceFileTmp.size())));
                    //APPENDSTRPTR(errorStrPtr_par, "Error, can't resume, destination file is bigger than source, destination size: " + QString::number(destinationFileTmp.size())
                    //             + " source size: "  + QString::number(sourceFileTmp.size()))
                    breakAfterTmp = true;
                    break;
                }

                //seek the source file at the same position as the destination (open "Append" seeks to the end of destination file)
                sourceFileTmp.seek(destinationFileTmp.pos());
                break;
            }

            if (copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::lazyResume)
            {
                //seek to the diff pos
                //if the source and destination are different from the first byte or destination doesn't exist
                //diffPos will be 0, which will be the same as create/overwrite
                sourceFileTmp.seek(diffPos_pri);
                destinationFileTmp.seek(diffPos_pri);
                break;
            }
            break;
        }

        if (breakAfterTmp)
        {
            break;
        }

        //would use size_t but qt uses qint64 making it unsigned vs signed, compiler complains
        qint64 bytesReadSourceTmp(0);
        qint64 bytesWrittenDestinationTmp(0);
        qint64 bufferSizeTmp(copyFileActionPtr_pri->bufferSize_f());
        if (bufferSizeTmp > (sourceFileTmp.size() - sourceFileTmp.pos()))
        {
            bufferSizeTmp = sourceFileTmp.size() - sourceFileTmp.pos();
        }

        do
        {
            if (pleaseStop_pri)
            {
                break;
            }
            std::string copyBufferTmp(bufferSizeTmp, '\0');
            bytesReadSourceTmp = sourceFileTmp.read(&copyBufferTmp[0], bufferSizeTmp);
            if (bytesReadSourceTmp == -1
                or bytesReadSourceTmp not_eq bufferSizeTmp)
            {
                APPENDSTRPTR(errorStrPtr_par, "Error when reading source file: " + source_par_con)
                break;
            }
            else
            {
                //good
            }
            bytesWrittenDestinationTmp = destinationFileTmp.write(&copyBufferTmp[0], bufferSizeTmp);
            if (bytesWrittenDestinationTmp == -1
                or bytesWrittenDestinationTmp not_eq bufferSizeTmp
                or bytesReadSourceTmp not_eq bytesWrittenDestinationTmp)
            {
                APPENDSTRPTR(errorStrPtr_par, "Error when writing destination file: " + destination_par_con)
                break;
            }
            else
            {
                //good
            }
            if (sourceFileTmp.bytesAvailable() < bufferSizeTmp)
            {
                bufferSizeTmp = sourceFileTmp.bytesAvailable();
            }
        }
        while (sourceFileTmp.bytesAvailable() > 0
               and bytesReadSourceTmp > 0
               //and bytesReadTmpA == bytesReadTmpB
               );

        break;
    }
}

void copyFileActionExecution_c::prepareCopyFile_f(
        const QFileInfo& sourceFileInfo_par_con
        , const QFileInfo& destinationFileInfo_par_con
        , QString* errorStrPtr_par
)
{
    while (true)
    {
//        MACRO_ADDACTONQTSOLOG("Source: " + sourceFileInfo_par_con.filePath()
//                              + " is dir " + QSTRINGBOOL(sourceFileInfo_par_con.isDir())
//                              + " is file " + QSTRINGBOOL(sourceFileInfo_par_con.isFile())
//                              , logItem_c::type_ec::debug);
//        MACRO_ADDACTONQTSOLOG("Destination: " + destinationFileInfo_par_con.filePath()
//                              + " is dir " + QSTRINGBOOL(destinationFileInfo_par_con.isDir())
//                              + " is file " + QSTRINGBOOL(destinationFileInfo_par_con.isFile())
//                              , logItem_c::type_ec::debug);
        if (destinationFileInfo_par_con.exists())
        {
            //destinationTreatment CASES:
            if (copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::noOverwrite)
            {
                //ignore and continue
                break;
            }

            if (copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::overwriteIsError)
            {
                //it's an error
                APPENDSTRPTR(errorStrPtr_par, "Overwrite is error, source " + sourceFileInfo_par_con.filePath() + " destination " + destinationFileInfo_par_con.filePath())
                break;
            }

            bool sameFileTmp(false);
            if (equalOnce_ft(copyFileActionPtr_pri->destinationTreatment_f()
                             , copyFileAction_c::destinationTreatment_ec::overwriteOnlyIfDifferent
                             , copyFileAction_c::destinationTreatment_ec::overwriteIsErrorOnlyIfDifferent
                             //lazy resume requires sameFile to get at which index the file is different
                             , copyFileAction_c::destinationTreatment_ec::lazyResume
                             )
                )
            {
                sameFileTmp = sameFile_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath());
            }

            if (not sameFileTmp and copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::overwriteIsErrorOnlyIfDifferent)
            {
                APPENDSTRPTR(errorStrPtr_par, "Files are different, overwrite error, source " + sourceFileInfo_par_con.filePath() + " destination " + destinationFileInfo_par_con.filePath())
                break;
            }

            //truemove first, since it's own beast
            if (copyFileActionPtr_pri->transferType_f() == copyFileAction_c::transferType_ec::trueMove
                and not equalOnce_ft(
                    copyFileActionPtr_pri->destinationTreatment_f()
                    , copyFileAction_c::destinationTreatment_ec::lazyResume
                    , copyFileAction_c::destinationTreatment_ec::tryStupidResume)
                )
            {
                blockMove_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath(), errorStrPtr_par);
                IFERRORSTRPTRNOTEMPTYBREAK(errorStrPtr_par)
                break;
            }

            //if it's not the same file and replace only if different
            //or unconditional overwrite
            if ((not sameFileTmp
                 and copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::overwriteOnlyIfDifferent)
                or copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::overwrite)
            {
                if (QFile::remove(destinationFileInfo_par_con.filePath()))
                {
                    //good
                }
                else
                {
                    APPENDSTRPTR(errorStrPtr_par, "Destination file could not be removed, to overwrite it after")
                    break;
                }

            }

            if (equalOnce_ft(copyFileActionPtr_pri->transferType_f()
                             , copyFileAction_c::transferType_ec::copy
                             , copyFileAction_c::transferType_ec::move
                             ))
            {
                blockCopy_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath(), errorStrPtr_par);
                IFERRORSTRPTRNOTEMPTYBREAK(errorStrPtr_par)
                break;
            }

            APPENDSTRPTR(errorStrPtr_par, "Invalid transfer type and destination treatment found");
        }
        else
        {
            //file destination doesn't exist, parent path might not exist too
            if (QFileInfo::exists(destinationFileInfo_par_con.path()))
            {
                //good
            }
            else
            {
                if (copyFileActionPtr_pri->createDestinationAndParents_f())
                {
                    QDir parentDir(destinationFileInfo_par_con.path());
                    parentDir.mkpath(".");
                }
                else
                {
                    APPENDSTRPTR(errorStrPtr_par, "Destination parent doesn't exist, destination parent: " + destinationFileInfo_par_con.path())
                    break;
                }
            }

            if (equalOnce_ft(copyFileActionPtr_pri->transferType_f()
                             , copyFileAction_c::transferType_ec::copy
                             , copyFileAction_c::transferType_ec::move
                             ))
            {
                blockCopy_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath(), errorStrPtr_par);
                IFERRORSTRPTRNOTEMPTYBREAK(errorStrPtr_par)
                break;
            }

            if (copyFileActionPtr_pri->transferType_f() == copyFileAction_c::transferType_ec::trueMove
                and not equalOnce_ft(copyFileActionPtr_pri->destinationTreatment_f()
                                     , copyFileAction_c::destinationTreatment_ec::lazyResume
                                     , copyFileAction_c::destinationTreatment_ec::tryStupidResume))
            {
                blockMove_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath(), errorStrPtr_par);
                IFERRORSTRPTRNOTEMPTYBREAK(errorStrPtr_par)
                break;
            }
        }
        break;
    }

    //"if" no errors
    while (errorStrPtr_par->isEmpty())
    {
        if (equalOnce_ft(copyFileActionPtr_pri->transferType_f()
                         , copyFileAction_c::transferType_ec::move
                         , copyFileAction_c::transferType_ec::trueMove))
        {
            //if the copy action is stopped, the copy/move functions will stop midway
            //and return and continue here going about removing the source, for the move,
            //before that check and break
            if (pleaseStop_pri)
            {
                break;
            }
            if (QFile::remove(sourceFileInfo_par_con.filePath()))
            {
                //good
            }
            else
            {
                APPENDSTRPTR(errorStrPtr_par, "After successful move could not remove source file: " + copyFileActionPtr_pri->sourcePathParsed_f())
                break;
            }
        }
        break;
    }
}

void copyFileActionExecution_c::derivedExecute_f()
{
    MACRO_ADDACTONQTSOLOG("Begin", logItem_c::type_ec::debug);
    QString errorStrTmp;
    while (copyFileActionPtr_pri->isValid_f(std::addressof(errorStrTmp)))
    {
        QFileInfo sourceFileInfoTmp(copyFileActionPtr_pri->sourcePathParsed_f());
        if (sourceFileInfoTmp.exists())
        {
            //good
        }
        else
        {
            errorStrTmp.append("Source doesn't exist");
            break;
        }

        bool sourceIsFileTmp(false);
        if (sourceFileInfoTmp.isFile())
        {
            sourceIsFileTmp = true;
        }
        bool sourceIsDirTmp(false);
        if (sourceFileInfoTmp.isDir())
        {
            sourceIsDirTmp = true;
            if (sourceFileInfoTmp.filePath().endsWith("/"))
            {
                //good
            }
            else
            {
                sourceFileInfoTmp.setFile(sourceFileInfoTmp.filePath() + "/");
            }
        }

        QFileInfo destinationFileInfoTmp(copyFileActionPtr_pri->destinationPathParsed_f());
        bool destinationIsFileTmp(false);
        bool destinationIsDirTmp(false);
        const bool destinationExistsTmp(destinationFileInfoTmp.exists());
        if (destinationExistsTmp)
        {
            destinationIsFileTmp = destinationFileInfoTmp.isFile();
            destinationIsDirTmp = destinationFileInfoTmp.isDir();
        }
        else
        {
            //case non existing destination
            //let's assume if source is file destination is file and if source is dir destination is dir
            //except if destination path string ends with "/"
            if (copyFileActionPtr_pri->destinationPathParsed_f().endsWith("/"))
            {
                destinationIsDirTmp = true;
                destinationIsFileTmp = false;
            }
            else
            {
                destinationIsFileTmp = sourceIsFileTmp;
                destinationIsDirTmp = sourceIsDirTmp;
            }
        }

        //because later path construction will require this
        if (destinationIsDirTmp and not destinationFileInfoTmp.filePath().endsWith("/"))
        {
            destinationFileInfoTmp.setFile(destinationFileInfoTmp.filePath() + "/");
        }

        MACRO_ADDACTONQTSOLOG("\nSource: " + sourceFileInfoTmp.filePath()
                              + " is dir " + QSTRINGBOOL(sourceIsDirTmp)
                              + " is file " + QSTRINGBOOL(sourceIsFileTmp)
                              , logItem_c::type_ec::debug);
        MACRO_ADDACTONQTSOLOG("\nDestination: " + destinationFileInfoTmp.filePath()
                              + " is dir " + QSTRINGBOOL(destinationIsDirTmp)
                              + " is file " + QSTRINGBOOL(destinationIsFileTmp)
                              , logItem_c::type_ec::debug);

        //case 1 source directory, destination file case
        if (sourceIsDirTmp and destinationIsFileTmp)
        {
            errorStrTmp.append("Source is a directory and destination is a file");
            break;
        }
        Q_EMIT executionStateChange_signal(actionExecutionState_ec::executing);

        //case 2 source is file and destination is file
        //"else" case 3 and 4, 3: source is directory and destination is dir. 4: source is file and destination is dir
        if (sourceIsFileTmp and destinationIsFileTmp)
        {
            prepareCopyFile_f(sourceFileInfoTmp, destinationFileInfoTmp, std::addressof(errorStrTmp));
            if (errorStrTmp.isEmpty())
            {
                //good
            }
            else
            {
                break;
            }
        }
        //source is file or directory and destination is directory
        else
        {
            if (destinationExistsTmp)
            {
                //good
            }
            else
            {
                if (copyFileActionPtr_pri->createDestinationAndParents_f())
                {
                    QDir dirTmp(destinationFileInfoTmp.filePath());
                    dirTmp.mkpath(".");
                }
                else
                {
                    errorStrTmp.append("Destination doesn't exist, destination: " + destinationFileInfoTmp.path());
                    break;
                }
            }

            if (sourceIsDirTmp)
            {
                std::vector<QString> fileListTmp(copyFileActionPtr_pri->testSourceFileList_f(std::addressof(errorStrTmp)));
                MACRO_ADDACTONQTSOLOG("FileList size " + QString::number(fileListTmp.size()), logItem_c::type_ec::info);
                if (errorStrTmp.isEmpty())
                {
                    //good
                }
                else
                {
                    break;
                }

                if (fileListTmp.empty())
                {
                    if (copyFileActionPtr_pri->noFilesCopiedIsError_f())
                    {
                        errorStrTmp.append("No file/s to copy found");
                    }
                    else
                    {
                       errorStrTmp.append("No file/s to copy found");
                    }
                    break;
                }
                for (const QString& fileStr_ite_con : fileListTmp)
                {
                    QFileInfo currentSourceFileInfoTmp(fileStr_ite_con);
                    QFileInfo currentDestinationFileInfoTmp(destinationFileInfoTmp.filePath() + fileStr_ite_con.mid(sourceFileInfoTmp.filePath().size()));
                    QDir parentDestinationDirTmp(currentDestinationFileInfoTmp.path());
                    if (parentDestinationDirTmp.exists())
                    {
                        //good
                    }
                    else
                    {
                        parentDestinationDirTmp.mkpath(".");
                    }
                    prepareCopyFile_f(currentSourceFileInfoTmp, currentDestinationFileInfoTmp, std::addressof(errorStrTmp));
                    if (errorStrTmp.isEmpty())
                    {
                        //good
                    }
                    else
                    {
                        if (copyFileActionPtr_pri->stopAllCopyOnFileCopyError_f())
                        {
                            break;
                        }
                    }
                    if (pleaseStop_pri)
                    {
                        break;
                    }
                }
                break;
            }
            //destination is dir
            if (sourceIsFileTmp)
            {
                QFileInfo currentDestinationFileInfoTmp(destinationFileInfoTmp.filePath() + sourceFileInfoTmp.fileName());
                prepareCopyFile_f(sourceFileInfoTmp, currentDestinationFileInfoTmp, std::addressof(errorStrTmp));
                break;
            }
        }
        break;
    }
    if (errorStrTmp.isEmpty())
    {
        //good
    }
    else
    {
        Q_EMIT addError_signal(errorStrTmp);
    }
    Q_EMIT anyFinish_signal();
}

void copyFileActionExecution_c::derivedStop_f()
{
    pleaseStop_pri = true;
    copyFileActionPtr_pri->stopDirectoryFiltering_f();
    {
        QMutexLocker mutexLockerTmp(std::addressof(checkSameFileMutex_pri));
        if (checkSameFile_ptr not_eq nullptr)
        {
            checkSameFile_ptr->stop_f();
        }
    }
}

void copyFileActionExecution_c::derivedKill_f()
{
    //no need
}


