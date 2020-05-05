#include "deleteFileDirExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actions/deleteFileDir.hpp"
#include "../actonDataHub.hpp"

#include <QFileInfo>
#include <QDir>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

deleteFiledirActionExecution_c::deleteFiledirActionExecution_c(
        actionDataExecutionResult_c* actionExecutionResultObj_par_con
        , deleteFileDirAction_c* deleteFileDirActionPtr_par
)
    : baseActionExecution_c(actionExecutionResultObj_par_con)
    , deleteFileDirActionPtr_pri(deleteFileDirActionPtr_par)
{}

void deleteFiledirActionExecution_c::derivedExecute_f()
{
    QFileInfo pathToDeleteTmp(deleteFileDirActionPtr_pri->pathParsed_f());
    while (true)
    {
        if (pathToDeleteTmp.exists())
        {
            if (pathToDeleteTmp.isDir())
            {
                QDir pathToDeleteDirTmp(deleteFileDirActionPtr_pri->pathParsed_f());
                auto itemCountTmp(pathToDeleteDirTmp.entryList(QDir::Files | QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot).count());
                if (deleteFileDirActionPtr_pri->onlyIfEmpty_f() and itemCountTmp > 0)
                {
#ifdef DEBUGJOUVEN
                    //qDebug() << "pathToDeleteDirTmp.count() " << pathToDeleteDirTmp.count() << endl;
#endif
                    if (itemCountTmp > 0)
                    {
                        text_c errorTmp("Path to delete {0} ignored, it contains {1} items" , deleteFileDirActionPtr_pri->pathParsed_f(), itemCountTmp);
                        MACRO_ADDACTONQTSOLOG(errorTmp, deleteFileDirActionPtr_pri, logItem_c::type_ec::warning);
                    }
                    //ignore, see onlyIfEmpty_f private variable description
                    //Q_EMIT addError_signal({"Can't delete {0}, directory is not empty", deleteFileDirActionPtr_pri->pathParsed_f()});
                    break;
                }
                else
                {
                    //it's either empty dir or "remove onlyIfEmpty_f" is false
                }

                if (pathToDeleteDirTmp.removeRecursively())
                {
                    //success
                    break;
                }
                else
                {
                    Q_EMIT addError_signal({"Error removing {0}", deleteFileDirActionPtr_pri->pathParsed_f()});
                    break;
                }
            }

            if (pathToDeleteTmp.isFile())
            {
                if (deleteFileDirActionPtr_pri->onlyIfEmpty_f() and pathToDeleteTmp.size() > 0)
                {
                    //ignore, see onlyIfEmpty_f private variable description
                    //Q_EMIT addError_signal({"Can't delete {0}, file is not empty", deleteFileDirActionPtr_pri->pathParsed_f()});
                    break;
                }
                else
                {
                    //it's empty or not "remove onlyIfEmpty_f"
                }

                if (QFile::remove(deleteFileDirActionPtr_pri->pathParsed_f()))
                {
                    //success
                    break;
                }
                else
                {
                    Q_EMIT addError_signal({"Error removing {0}", deleteFileDirActionPtr_pri->pathParsed_f()});
                    break;
                }
            }

            Q_EMIT addError_signal({"Can't delete {0}, path is not a file or folder", deleteFileDirActionPtr_pri->pathParsed_f()});
            break;

        }
        else
        {
            if (deleteFileDirActionPtr_pri->errorIfNotExists_f())
            {
                Q_EMIT addError_signal({"Can't delete {0}, path doesn't exists", deleteFileDirActionPtr_pri->pathParsed_f()});
                break;
            }
            else
            {
                //it's fine
            }
        }
        break;
    }
    Q_EMIT anyFinish_signal();
}

void deleteFiledirActionExecution_c::derivedStop_f()
{
    //no need
}

void deleteFiledirActionExecution_c::derivedKill_f()
{
    //no need
}


