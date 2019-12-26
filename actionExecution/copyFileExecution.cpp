#include "copyFileExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actions/copyFile.hpp"
#include "../reused/sameFileAlgo.hpp"
#include "../actonDataHub.hpp"

#include "essentialQtso/macros.hpp"
#include "filterDirectoryQtso/crossPlatformMacros.hpp"
#include "textQtso/text.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

#include <QMap>

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
    {
        QMutexLocker mutexLockerTmp(std::addressof(checkSameFileMutex_pri));
        checkSameFile_ptr = std::addressof(checkSameFileTmp);
    }
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
        , textCompilation_c* errorsPtr_par
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
            text_c errorTextTmp("Error when opening source file: {0}", source_par_con);
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            break;
        }

        QFile destinationFileTmp(destination_par_con);
        if (destinationFileTmp.open(QIODevice::ReadWrite))
        {
            //good
        }
        else
        {
            text_c errorTextTmp("Error when opening destination file: {0}", destination_par_con);
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            break;
        }

        //would use size_t but qt uses qint64 making it unsigned vs signed, compiler complains
        qint64 bytesReadSourceTmp(0);
        qint64 bytesWrittenDestinationTmp(0);
        qint64 bufferSizeTmp(copyFileActionPtr_pri->bufferSize_f());

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
                        text_c errorTextTmp("Error when seeking destination file: {0}", destination_par_con);
                        APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
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
                        text_c errorTextTmp("Error when reading destination file: {0}", destination_par_con);
                        APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
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
                        text_c errorTextTmp("Error when seeking destination file: {0}", destination_par_con);
                        APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                        break;
                    }
                    //write
                    int_fast64_t destinationWrittenSizeTmp(destinationFileTmp.write(&destinationBufferTmp[0], destinationBufferSizeTmp));
                    if (destinationWrittenSizeTmp == -1
                        or destinationWrittenSizeTmp not_eq destinationBufferSizeTmp
                        or destinationWrittenSizeTmp not_eq destinationReadSizeTmp)
                    {
                        text_c errorTextTmp("Error when writing destination file: {0}", destination_par_con);
                        APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
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
                text_c errorTextTmp("Error seeking source file: {0}", source_par_con);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
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
                text_c errorTextTmp("Error when reading source file: {0}", source_par_con);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
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
                text_c errorTextTmp("Error when writing destination file: {0}", destination_par_con);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
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
                text_c errorTextTmp("Error resizing source file: {0}", source_par_con);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                break;
            }
        }
        break;
    }
}

QString numberToStr_f(
        //number to convert to string
        const int_fast64_t number_par_con
        //final length/size of the string, 0 will not try to prepend padding
        , const int_fast64_t numberStrSize_par_con = 0
        //padding character to reach previous argument string length/size
        , const QChar& paddingQChar_par_con = '0'
)
{
    QString resultTmp(QString::number(number_par_con));
    if (numberStrSize_par_con > 0 and resultTmp.size() < numberStrSize_par_con)
    {
        resultTmp.prepend(QString(numberStrSize_par_con - resultTmp.size(), paddingQChar_par_con));
    }
    return resultTmp;
}


void copyFileActionExecution_c::blockMoveFiles_f(
        const QString& source_par_con
        , const QString& destination_par_con
        , textCompilation_c* errorsPtr_par)
{
    //move by file-blocks, tries to resume by default,
    //but only if the previous transfer used this method and the source hasn't changed,
    //the destination file-blocks haven't changed, otherwise it will go wrong
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
            text_c errorTextTmp("Error when opening source file: {0}", source_par_con);
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            break;
        }

        QFileInfo destinationFileInfoTmp(destination_par_con);

        //check if there are moved files on the destination already
        filterOptions_s filterOptionsTmp;
        filterOptionsTmp.navigateSubdirectories_pub = false;
        filterOptionsTmp.useAbsolutePaths_pub = false;
        filterOptionsTmp.listFiles_pub = true;
        filterOptionsTmp.listHidden_pub = true;
        filterOptionsTmp.navigateHiddenDirectories_pub = false;
        filterOptionsTmp.listDirectories_pub = false;
        //parts are like %originalFilename%.001, %originalFilename%.002
        filterOptionsTmp.filenameRegexFilters_pub.append(destinationFileInfoTmp.fileName() + "\\.[0-9]+$");
        filterOptionsTmp.listEmptyDirectories_pub = false;

        directoryFilter_c directoryFilterTmp(destinationFileInfoTmp.path(), filterOptionsTmp);
        std::vector<QString> fileListResultTmp(directoryFilterTmp.filter_f());

        //check if file size is multiple of buffersize
        const int_fast64_t remainingBytesTmp_con(sourceCurrentFileSizeTmp % copyFileActionPtr_pri->bufferSize_f());
        //calculate the total file parts
        const int_fast64_t destinationFilePartsCountTmp_con(
                    ((remainingBytesTmp_con == 0) ? 0 : 1) +
                    (sourceCurrentFileSizeTmp / copyFileActionPtr_pri->bufferSize_f())
        );

        //if resumeIndexTmp == destinationFilePartsCountTmp_con it's at/past the end,
        //to be able to read it must be decreased by 1
        //resumeIndex gets initialized with the last index value
        int_fast64_t resumeIndexTmp(destinationFilePartsCountTmp_con - 1);

        //determine how many digits will take the largest index to know how many
        //'0' must be used when padding the lower indexes
        const int_fast32_t largestIndexStrSizeTmp_con(QString::number(resumeIndexTmp).size());

        if (fileListResultTmp.empty())
        {
            //no previous moved files found
        }
        else
        {
            //found previous moved files
            //check where to resume based on the first part
            //with the lowest index that's equal to the source at that
            //index (seek source bufferSize * index)

            //gather all the moved indexes from the filenames
            QMap<int_fast64_t, QString> partIndexesTmp;
            for (const QString& filePartname_ite_con : fileListResultTmp)
            {
                auto lastDotIndexTmp(filePartname_ite_con.lastIndexOf('.'));
                partIndexesTmp.insert(filePartname_ite_con.mid(lastDotIndexTmp + 1).toLongLong(), filePartname_ite_con);
            }

            //only one part in the destination, "should" be the remaining bytes of the end
            if (partIndexesTmp.size() == 1)
            {
                //since doing this-seebelow already takes more time than copying the first part (starting from the end) again
                //just remove this one part and start again
                //.first() returns the first value (there is also firstKey) I wonder why it is not named firstValue(),
                //which is the filename of this "only" part
                QFile::remove(partIndexesTmp.first());

//                //seek source near the end
//                sourceFileTmp.seek(copyFileActionPtr_pri->bufferSize_f() * destinationFilePartsCountTmp_con);
//                std::string sourceBufferTmp(remainingBytesTmp_con, '\0');
//                if (remainingBytesTmp_con == 0)
//                {
//                    sourceBufferTmp.resize(copyFileActionPtr_pri->bufferSize_f(), '\0');
//                }

//                //read source "end"
//                sourceFileTmp.read(&sourceBufferTmp[0], sourceBufferTmp.size());

//                //read the destination file
//                QFile destinationFileTmp(partIndexesTmp.first());
//                if (destinationFileTmp.open(QIODevice::ReadWrite))
//                {
//                    std::string destinationBufferTmp(sourceFileTmp.size(), '\0');
//                    destinationFileTmp.read(&destinationBufferTmp[0], destinationBufferTmp.size());

//                    if (sourceBufferTmp == destinationBufferTmp)
//                    {
//                        resumeIndexTmp = destinationFilePartsCountTmp_con - 1;
//                    }
//                }
//                else
//                {

//                }
            }
            else
            {
                //in this codepath partIndexesTmp will always be >1
                //compare source and destination parts, starting by the largest index and go down, until something IS EQUAL

                //function call-name wise constBegin and cbegin are the equal, same with cend and constEnd
                QMap<int_fast64_t, QString>::const_iterator iteratorTmp(partIndexesTmp.cend());
                while (true)
                {
                    --iteratorTmp;

                    //seek source to index where the part was read
                    sourceFileTmp.seek(copyFileActionPtr_pri->bufferSize_f() * iteratorTmp.key());

                    //read source
                    std::vector<char> sourceBufferTmp(copyFileActionPtr_pri->bufferSize_f());
                    sourceFileTmp.read(&sourceBufferTmp[0], sourceBufferTmp.size());

                    //read destination file
                    QFile destinationFileTmp(iteratorTmp.value());
                    if (destinationFileTmp.open(QIODevice::ReadWrite))
                    {
                        std::vector<char> destinationBufferTmp(sourceFileTmp.size());
                        destinationFileTmp.read(&destinationBufferTmp[0], destinationBufferTmp.size());
                        if (sourceBufferTmp == destinationBufferTmp)
                        {
                            //IMPORTANT it's presumed that when an equal part is found in the destination,
                            //all the others parts below are OK-equal too
                            resumeIndexTmp = destinationFilePartsCountTmp_con - 1;
                            break;
                        }
                    }
                    else
                    {
                        //ignore file open error and try with next file-block
                    }

                    if (iteratorTmp == partIndexesTmp.cbegin())
                    {
                        break;
                    }
                }
            }
        }

        //start moving, read source
        //and create the destination file parts
        while (resumeIndexTmp not_eq -1)
        {
            //seek source to index where the part is to be read
            sourceFileTmp.seek(copyFileActionPtr_pri->bufferSize_f() * resumeIndexTmp);

            //read source
            std::vector<char> sourceBufferTmp;
            //if the index is on the last part, resize the buffer
            if (resumeIndexTmp == (destinationFilePartsCountTmp_con - 1))
            {
                sourceBufferTmp.resize(remainingBytesTmp_con);
            }
            else
            {
                sourceBufferTmp.resize(copyFileActionPtr_pri->bufferSize_f());
            }
            auto readResultTmp(sourceFileTmp.read(&sourceBufferTmp[0], sourceBufferTmp.size()));
            if (readResultTmp == -1)
            {
                text_c errorTextTmp("Error reading source file: {0}", source_par_con);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                break;
            }

            //write destination file
            QString destinationFilePartTmp(destination_par_con + '.' + numberToStr_f(resumeIndexTmp, largestIndexStrSizeTmp_con));
            QFile destinationFileTmp(destinationFilePartTmp);
            if (destinationFileTmp.open(QIODevice::WriteOnly))
            {
                auto writingResultTmp(destinationFileTmp.write(&sourceBufferTmp[0], sourceBufferTmp.size()));
                if (writingResultTmp == -1)
                {
                    text_c errorTextTmp("Error writing destination file part: {0}", destinationFilePartTmp);
                    APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                    break;
                }
            }
            else
            {
                text_c errorTextTmp("Error opening destination file part: {0}", destinationFilePartTmp);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                break;
            }
            //resize the source file
            sourceFileTmp.resize(copyFileActionPtr_pri->bufferSize_f() * resumeIndexTmp);
            //decrease the file-resume-index
            resumeIndexTmp = resumeIndexTmp - 1;
        }

        //remove source
        if (resumeIndexTmp == -1)
        {
            if (sourceFileTmp.remove())
            {
                //good
            }
            else
            {
                text_c errorTextTmp("Error removing source file: {0}", source_par_con);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                break;
            }
        }
        else
        {
            //some error happened
            break;
        }

        //join destination files

        //open-create final destination file
        QFile finalDestinationFileTmp(destination_par_con);
        if (finalDestinationFileTmp.open(QIODevice::WriteOnly))
        {
            //iterate-read the destination file parts in ascending order
            for (int_fast64_t i = 0; i < destinationFilePartsCountTmp_con; ++i)
            {
                //open destination file part
                QString destinationFilePartStrTmp(destination_par_con + '.' + numberToStr_f(i, largestIndexStrSizeTmp_con));
                QFile destinationFilePartTmp(destinationFilePartStrTmp);
                if (destinationFilePartTmp.open(QIODevice::ReadOnly))
                {
                    std::vector<char> destinationPartBufferTmp(destinationFilePartTmp.size());
                    auto readResultTmp(destinationFilePartTmp.read(&destinationPartBufferTmp[0], destinationPartBufferTmp.size()));
                    if (readResultTmp == -1)
                    {
                        text_c errorTextTmp("Error reading destination file part: {0}", destinationFilePartStrTmp);
                        APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                        break;
                    }
                    //write to final destination file
                    auto writingResultTmp(finalDestinationFileTmp.write(&destinationPartBufferTmp[0], destinationPartBufferTmp.size()));
                    if (writingResultTmp == -1)
                    {
                        text_c errorTextTmp("Error writing destination file: {0}", destination_par_con);
                        APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                        break;
                    }
                    //remove part
                    if (destinationFilePartTmp.remove())
                    {
                        //good
                    }
                    else
                    {
                        text_c errorTextTmp("Error removing destination file part: {0}", destinationFilePartStrTmp);
                        APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                        break;
                    }
                }
                else
                {
                    text_c errorTextTmp("Error opening destination file part: {0}", destinationFilePartStrTmp);
                    APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                    break;
                }
            }
        }
        else
        {
            text_c errorTextTmp("Error when opening destination file: {0}", destination_par_con);
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            break;
        }

        break;
    }
}

//source file must exist, destination may or may not exist
void copyFileActionExecution_c::blockCopy_f(
        const QString& source_par_con
        , const QString& destination_par_con
        , textCompilation_c* errorsPtr_par
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
            text_c errorTextTmp("Error when opening source file: {0}", source_par_con);
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            break;
        }

        QFile destinationFileTmp(destination_par_con);
        bool isResumeTmp(
                    copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::resume
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
            text_c errorTextTmp("Error when opening/creating destination file: {0}", destination_par_con);
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            break;
        }

        bool breakAfterTmp(false);
        while (isResumeTmp)
        {
            if (copyFileActionPtr_pri->resumeType_f() == copyFileAction_c::resumeType_ec::stupid)
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
                    Q_EMIT addOutput_signal({"Destination file {0}"
                                             " is bigger than source {1}"
                                             ", destination size: {2}"
                                             ", source size: {3} "
                                             , destination_par_con
                                             , source_par_con
                                             , destinationFileTmp.size()
                                             , sourceFileTmp.size()}
                    );
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

            if (copyFileActionPtr_pri->resumeType_f() == copyFileAction_c::resumeType_ec::lazy)
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
                text_c errorTextTmp("Error when reading source file: {0}", source_par_con);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
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
                text_c errorTextTmp("Error when writing destination file: {0}", destination_par_con);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
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
        , textCompilation_c* errorsPtr_par
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
                text_c errorTextTmp("Overwrite is error, source {0} destination {1}", sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath());
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                break;
            }

            bool sameFileTmp(false);
            if (equalOnce_ft(
                    copyFileActionPtr_pri->destinationTreatment_f()
                    , copyFileAction_c::destinationTreatment_ec::overwriteOnlyIfDifferent
                    , copyFileAction_c::destinationTreatment_ec::overwriteIsErrorOnlyIfDifferent
                    )
                or (
                    copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::resume
                    and copyFileActionPtr_pri->resumeType_f() == copyFileData_c::resumeType_ec::lazy
                    )
                )
            {
                sameFileTmp = sameFile_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath());
            }

            if (sameFileTmp)
            {
                break;
            }
            else
            {
                if (copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::overwriteIsErrorOnlyIfDifferent)
                {
                    text_c errorTextTmp("Files are different, overwrite error, source {0} destination {1}", sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath());
                    APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                    break;
                }
            }



            //if it's not the same file and replace only if different
            //or unconditional overwrite
            if (copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::overwriteOnlyIfDifferent
                or copyFileActionPtr_pri->destinationTreatment_f() == copyFileAction_c::destinationTreatment_ec::overwrite)
            {
                if (QFile::remove(destinationFileInfo_par_con.filePath()))
                {
                    //good
                }
                else
                {
                    text_c errorTextTmp("Destination file {0} could not be removed, to overwrite it after", destinationFileInfo_par_con.filePath());
                    APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                    break;
                }

            }


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
                    text_c errorTextTmp("Destination parent doesn't exist, destination parent: {0}", destinationFileInfo_par_con.filePath());
                    APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                    break;
                }
            }
        }

        //special case for files equal-smaller than the bufferSize, just use regular copy, ignoring transferType
        if (sourceFileInfo_par_con.size() <= copyFileActionPtr_pri->bufferSize_f())
        {
            blockCopy_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath(), errorsPtr_par);
            break;
        }

        //truemoveFiles
        if (copyFileActionPtr_pri->transferType_f() == copyFileAction_c::transferType_ec::trueMoveFiles
            and  copyFileActionPtr_pri->resumeType_f() == copyFileAction_c::resumeType_ec::transferTypeImplementation
            )
        {
            blockMoveFiles_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath(), errorsPtr_par);
            IFERRORTEXTPTRNOTEMPTYBREAK(errorsPtr_par)
            break;
        }

        //truemove
        if (copyFileActionPtr_pri->transferType_f() == copyFileAction_c::transferType_ec::trueMove
            and  copyFileActionPtr_pri->resumeType_f() == copyFileAction_c::resumeType_ec::transferTypeImplementation
            )
        {
            blockMove_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath(), errorsPtr_par);
            IFERRORTEXTPTRNOTEMPTYBREAK(errorsPtr_par)
            break;
        }

        if (equalOnce_ft(copyFileActionPtr_pri->transferType_f()
                         , copyFileAction_c::transferType_ec::copy
                         , copyFileAction_c::transferType_ec::move
                         ))
        {
            blockCopy_f(sourceFileInfo_par_con.filePath(), destinationFileInfo_par_con.filePath(), errorsPtr_par);
            IFERRORTEXTPTRNOTEMPTYBREAK(errorsPtr_par)
            break;
        }

        APPENDTEXTPTR(errorsPtr_par, "Invalid transfer type - destination treatment - resume type found");

        break;
    }

    //"if" no errors
    while (errorsPtr_par->empty_f())
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
                text_c errorTextTmp("After successful move could not remove source file: {0}", copyFileActionPtr_pri->sourcePathParsed_f());
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                break;
            }
        }
        break;
    }
}

void copyFileActionExecution_c::executeSinglePath_f(
        const QString& path_par_con
        , textCompilation_c* errorsPtr_par)
{
    while (true)
    {
        QFileInfo sourceFileInfoTmp(path_par_con);
        if (sourceFileInfoTmp.exists())
        {
            //good
        }
        else
        {
            text_c errorTextTmp("Source doesn't exist {0}", sourceFileInfoTmp.filePath());
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
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

        {
            text_c logTextTmp("\nSource: {0} is dir {1} is file {2}", sourceFileInfoTmp.filePath(), QSTRINGBOOL(sourceIsDirTmp), QSTRINGBOOL(sourceIsFileTmp));
            MACRO_ADDACTONQTSOLOG(logTextTmp, logItem_c::type_ec::debug);
        }

        {
            text_c logTextTmp("\nDestination: {0} is dir {1} is file {2}", destinationFileInfoTmp.filePath(), QSTRINGBOOL(destinationIsDirTmp), QSTRINGBOOL(destinationIsFileTmp));
            MACRO_ADDACTONQTSOLOG(logTextTmp, logItem_c::type_ec::debug);
        }

        //case 1 source directory, destination file case
        if (sourceIsDirTmp and destinationIsFileTmp)
        {
            text_c errorTextTmp("Source {0} is a directory and destination {1} is a file", sourceFileInfoTmp.filePath(), destinationFileInfoTmp.path());
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            break;
        }

        //case 2 source is file and destination is file
        //"else" case 3 and 4, 3: source is directory and destination is dir. 4: source is file and destination is dir
        if (sourceIsFileTmp and destinationIsFileTmp)
        {
            prepareCopyFile_f(sourceFileInfoTmp, destinationFileInfoTmp, errorsPtr_par);
            if (errorsPtr_par->empty_f())
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
                    text_c errorTextTmp("Destination {0} doesn't exist", destinationFileInfoTmp.path());
                    APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                    break;
                }
            }

            if (sourceIsDirTmp)
            {
                std::vector<QString> fileListTmp(copyFileData_c::testSourceFileList_f(
                                                     copyFileActionPtr_pri
                                                     , directoryFilterPtr_pri
                                                     , errorsPtr_par
                                                     , std::addressof(directoryFilteringMutex_pri)
                                                     )
                );
                text_c logTextTmp("FileList size {0}", fileListTmp.size());
                MACRO_ADDACTONQTSOLOG(logTextTmp, logItem_c::type_ec::info);
                if (errorsPtr_par->empty_f())
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
                        text_c errorTextTmp("No file/s to copy found in the source {0}", sourceFileInfoTmp.filePath());
                        APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                    }
                    else
                    {
                        text_c textTmp("No file/s to copy found in the source {0}", sourceFileInfoTmp.filePath());
                        Q_EMIT addOutput_signal(textTmp);
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
                    prepareCopyFile_f(currentSourceFileInfoTmp, currentDestinationFileInfoTmp, errorsPtr_par);
                    if (errorsPtr_par->empty_f())
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
                prepareCopyFile_f(sourceFileInfoTmp, currentDestinationFileInfoTmp, errorsPtr_par);
                break;
            }
        }
        break;
    }
}

void copyFileActionExecution_c::derivedExecute_f()
{
    MACRO_ADDACTONQTSOLOG("Begin copy file action execution", logItem_c::type_ec::debug);
    textCompilation_c errorsTmp;
    while (copyFileActionPtr_pri->isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        QString sourceParsedTmp(copyFileActionPtr_pri->sourcePathParsed_f());
        if (sourceParsedTmp.contains('\n'))
        {
            QStringList splitResulTmp(sourceParsedTmp.split('\n', QString::SkipEmptyParts));
            for (const QString& sourceStr_ite_con : splitResulTmp)
            {
                executeSinglePath_f(sourceStr_ite_con, std::addressof(errorsTmp));
                if (errorsTmp.empty_f())
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
            }
        }
        else
        {
            executeSinglePath_f(sourceParsedTmp, std::addressof(errorsTmp));
        }
        break;
    }
    if (errorsTmp.empty_f())
    {
        //good
    }
    else
    {
        Q_EMIT addErrors_signal(errorsTmp);
    }
    Q_EMIT anyFinish_signal();
}

void copyFileActionExecution_c::derivedStop_f()
{
    pleaseStop_pri = true;
    copyFileData_c::stopDirectoryFiltering_f(directoryFilterPtr_pri, std::addressof(directoryFilteringMutex_pri));
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


