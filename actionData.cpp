#include "actionData.hpp"

#include "mappings/actionStrMapping.hpp"
#include "actionExecution/runProcessExecution.hpp"
#include "actionExecution/createDirectoryExecution.hpp"

//#include "criptoQtso/hashQt.hpp"
//#include "signalso/signal.hpp"

#include <QJsonArray>
//#include <QFileInfo>
//#include <QDir>
#include <QDateTime>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

int_fast64_t nextActionDataId_f()
{
    static int_fast64_t rootId(0);
    rootId = rootId + 1;
//    if (rootId == 2)
//    {
//        throw;
//    }
    return rootId;
}

//////////////////
//actionDataExecutionResult_c
//////////////////


QString actionDataExecutionResult_c::error_f() const
{
    return errors_pri;
}

QString actionDataExecutionResult_c::externalOutput_f() const
{
    return actionOutput_pri;
}

QString actionDataExecutionResult_c::externalErrorOutput_f() const
{
    return actionErrorOutput_pri;
}

int actionDataExecutionResult_c::returnCode_f() const
{
    return returnCode_pri;
}

actionExecutionState_ec actionDataExecutionResult_c::actionState_f() const
{
    return actionState_pri;
}

bool actionDataExecutionResult_c::returnCodeSet_f() const
{
    return returnCodeSet_pri;
}

const actionData_c& actionDataExecutionResult_c::parent_f() const
{
    return parent_pri;
}

bool actionDataExecutionResult_c::anyFinish_f() const
{
    return anyFinish_pri;
}


qint64 actionDataExecutionResult_c::startTime_f() const
{
    return startTime_pri;
}

bool actionDataExecutionResult_c::startTimeSet_f() const
{
    return startTimeSet_pri;
}

qint64 actionDataExecutionResult_c::finishedTime_f() const
{
    return finishedTime_pri;
}

bool actionDataExecutionResult_c::finishedTimeSet_f() const
{
    return finishedTimeSet_pri;
}

actionDataExecutionResult_c::actionDataExecutionResult_c(
        const actionData_c& parentActionData_par_con
        )
    : parent_pri(parentActionData_par_con)
{
}

QString actionDataExecutionResult_c::output_f() const
{
    return output_pri;
}

void actionDataExecutionResult_c::appendOutput_f(const QString& output_par_con)
{
    output_pri.append(output_par_con);
    Q_EMIT outputUpdated_signal(parent_pri.id_f());
}

void actionDataExecutionResult_c::appendError_f(const QString& error_par_con)
{
    errors_pri.append(error_par_con);
    Q_EMIT errorUpdated_signal(parent_pri.id_f());
}

void actionDataExecutionResult_c::appendExternalOutput_f(const QString& actionOutput_par_con)
{
    actionOutput_pri.append(actionOutput_par_con);
    Q_EMIT externalOutputUpdated_signal(parent_pri.id_f());
}

void actionDataExecutionResult_c::appendExternalError_f(const QString& actionError_par_con)
{
    actionErrorOutput_pri.append(actionError_par_con);
    Q_EMIT externalErrorUpdated_signal(parent_pri.id_f());
}

void actionDataExecutionResult_c::setReturnCode_f(const int returnCode_par_con)
{
    if (not returnCodeSet_pri)
    {
        returnCode_pri = returnCode_par_con;
        returnCodeSet_pri = true;
        Q_EMIT returnCodeUpdated_signal(parent_pri.id_f());
    }
}

void actionDataExecutionResult_c::setExecutionState_f(const actionExecutionState_ec actionExecutionState_par_con)
{
    actionState_pri = actionExecutionState_par_con;
    if (actionExecutionState_par_con == actionExecutionState_ec::running)
    {
        startTime_pri = QDateTime::currentMSecsSinceEpoch();
        startTimeSet_pri = true;
    }
    Q_EMIT executionStateUpdated_signal(parent_pri.id_f());
}

void actionDataExecutionResult_c::setAnyFinish_f()
{
    anyFinish_pri = true;
    finishedTime_pri = QDateTime::currentMSecsSinceEpoch();
    finishedTimeSet_pri = true;
    if (parent_pri.haltOnFail_f() and not errors_pri.isEmpty())
    {
        Q_EMIT halt_signal();
    }
    Q_EMIT anyFinish_signal(parent_pri.id_f());
}

//////////////////
//actionData_c
//////////////////


int_fast64_t actionData_c::id_f() const
{
    return id_pri;
}

action_ec actionData_c::type_f() const
{
    return type_pri;
}

QString actionData_c::description_f() const
{
    return description_pri;
}

bool actionData_c::haltOnFail_f() const
{
    return haltOnExecutionFail_pri;
}

QJsonObject actionData_c::actionDataJSON_f() const
{
    return actionDataJSON_pri;
}

void actionData_c::setType_f(const action_ec type_par_con)
{
    type_pri = type_par_con;
}

void actionData_c::setDescription_f(const QString& description_par_con)
{
    description_pri = description_par_con;
}

void actionData_c::setHaltOnFail_f(const bool haltOnFail_par_con)
{
    haltOnExecutionFail_pri = haltOnFail_par_con;
}

void actionData_c::setActionDataJSON_f(const QJsonObject& actionDataJSON_par_con)
{
    actionDataJSON_pri = actionDataJSON_par_con;
}

bool actionData_c::createExecution_f(actionDataExecutionResult_c* actionDataExecutionResult_par)
{
    bool resultTmp(false);
    while (true)
    {
        if (actionDataExecutionResult_par == Q_NULLPTR)
        {
            break;
        }

        deleteUsedPtrs_f();

        actionDataExecutionResult_pri = actionDataExecutionResult_par;

        switch (type_pri)
        {
            case action_ec::runProcess:
            {
                runProcessAction_c runProcessActionTmp;
                runProcessActionTmp.read_f(actionDataJSON_f());
                runProcessActionExecution_c* runProcessActionExecutionTmp = new runProcessActionExecution_c(runProcessActionTmp);

                //this will be necessary to stop the action by the user
                //the only thing a user can do when an action execution starts is
                //let it's "normal" course or try to stop it
                //FUTURE pause execution?
                executionObject_pri = runProcessActionExecutionTmp;

                QObject::connect(runProcessActionExecutionTmp, &runProcessActionExecution_c::executionStateChange_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::setExecutionState_f);
                QObject::connect(runProcessActionExecutionTmp, &runProcessActionExecution_c::addError_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::appendError_f);
                //since each process is different an unknown to this program no signal for slot appendOutput_f
                QObject::connect(runProcessActionExecutionTmp, &runProcessActionExecution_c::addProcessError_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::appendExternalError_f);
                QObject::connect(runProcessActionExecutionTmp, &runProcessActionExecution_c::addProcessOutput_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::appendExternalOutput_f);
                QObject::connect(runProcessActionExecutionTmp, &runProcessActionExecution_c::setReturnCode_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::setReturnCode_f);
                QObject::connect(runProcessActionExecutionTmp, &runProcessActionExecution_c::anyFinish_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::setAnyFinish_f);

                runProcessActionExecutionTmp->execute_f();
                resultTmp = true;
            }
                break;
            case action_ec::createDirectory:
            {
                createDirectoryAction_c createDirectoryActionTmp;
                createDirectoryActionTmp.read_f(actionDataJSON_f());

                createDirectoryActionExecution_c* createDirectoryActionExecutionTmp = new createDirectoryActionExecution_c(createDirectoryActionTmp);
                executionObject_pri = createDirectoryActionExecutionTmp;

                QObject::connect(createDirectoryActionExecutionTmp, &createDirectoryActionExecution_c::executionStateChange_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::setExecutionState_f);
                QObject::connect(createDirectoryActionExecutionTmp, &createDirectoryActionExecution_c::addError_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::appendError_f);
                QObject::connect(createDirectoryActionExecutionTmp, &createDirectoryActionExecution_c::addOutput_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::appendOutput_f);
                QObject::connect(createDirectoryActionExecutionTmp, &createDirectoryActionExecution_c::anyFinish_signal, actionDataExecutionResult_pri, &actionDataExecutionResult_c::setAnyFinish_f);

                createDirectoryActionExecutionTmp->execute_f();
                resultTmp = true;
            }
                break;
            default:
            {
                //theoretically it shouldn't enter here ever
            }
        }
        break;
    }
    return resultTmp;
}


actionDataExecutionResult_c* actionData_c::actionDataExecutionResultPtr_f() const
{
    return actionDataExecutionResult_pri;
}

void actionData_c::deleteUsedPtrs_f()
{
    if (executionObject_pri != Q_NULLPTR)
    {
        executionObject_pri->deleteLater();
        executionObject_pri = Q_NULLPTR;
    }
    if (actionDataExecutionResult_pri != Q_NULLPTR)
    {
        actionDataExecutionResult_pri->deleteLater();
        actionDataExecutionResult_pri = Q_NULLPTR;
    }
}

actionData_c::actionData_c()
    : id_pri(nextActionDataId_f())
{}

actionData_c::actionData_c(
        const action_ec type_par_con
        , const QString& description_par_con
        , const bool haltOnFail_par_con
        , const QJsonObject& actionDataJson_par_con)
    : id_pri(nextActionDataId_f())
    , type_pri(type_par_con)
    , description_pri(description_par_con)
    , haltOnExecutionFail_pri(haltOnFail_par_con)
    , actionDataJSON_pri(actionDataJson_par_con)
{
}

actionData_c::~actionData_c()
{
#ifdef DEBUGJOUVEN
    qDebug() << "~actionData_c()" << endl;
#endif
    deleteUsedPtrs_f();
}

void actionData_c::write_f(QJsonObject& json) const
{
    json["type"] = actionToStrUMap_glo_sta_con.at(type_pri);
    json["description"] = description_pri;
    json["actionDataJSON"] = actionDataJSON_pri;
    json["haltOnExecutionFail"] = haltOnExecutionFail_pri;

}

void actionData_c::read_f(const QJsonObject& json)
{
    type_pri = strToActionUMap_glo_sta_con.value(json["type"].toString());
    description_pri = json["description"].toString();
    actionDataJSON_pri = json["actionDataJSON"].toObject();
    haltOnExecutionFail_pri = json["haltOnExecutionFail"].toBool();
}

//////////////////
//actionDataController_c
//////////////////

bool actionDataController_c::moveRowActionData(
    const int sourceRow_par_con
    , const int destinationRow_par_con)
{
    bool resultTmp(false);

    int_fast32_t actionDataIdTmp(rowToActionDataId_f(sourceRow_par_con));
    //qInfo() << "actionDataIdTmp " << actionDataIdTmp << endl;
    if (actionDataIdTmp > 0)
    {
        //save the actionData stuff that is going to get moved
        auto actionDataIteratorTmp(actionData_f(actionDataIdTmp));
        //no ref because... first it's removed (removeActionDataUsingRow_f)
        //so copy
        actionData_c sourceActionDataTmp(actionDataIteratorTmp.first);

        //qInfo() << "sourceActionDataTmp.id_f() " << sourceActionDataTmp.id_f() << endl;

        //remove it using source row
        removeActionDataUsingRow_f(sourceRow_par_con, sourceActionDataTmp.id_f());
        //insert it in the destination row
        insertActionData_f(sourceActionDataTmp, destinationRow_par_con);

        resultTmp = true;
    }
    return resultTmp;
}

////for the drag and drop
//void actionDataController_c::moveRowActionData(
//    const int sourceRow_par_con
//    , const int destinationRow_par_con)
//{
//    if (sourceRow_par_con == destinationRow_par_con or sourceRow_par_con < 0 or destinationRow_par_con < 0
//        or sourceRow_par_con > rowToActionDataId_pri.size() or destinationRow_par_con > rowToActionDataId_pri.size())
//    {
//        return;
//    }
//    if (sourceRow_par_con > destinationRow_par_con)
//    {
//        //the source is further down than the destination, so move all the rows from source to destination+1 and then replace the destination
//        //to lose only one value, start from the source and move "up"

//        //back source id since it will be replaced by the first moved row (the one above)
//        auto sourceDataIdBackup(rowToActionDataId_pri.at(sourceRow_par_con));

//        auto indexTmp(sourceRow_par_con);
//        while (indexTmp > destinationRow_par_con)
//        {
//            //replace the row with the one above, current row = index, row above = index -1
//            auto movedActionIdTmp(rowToActionDataId_pri[indexTmp - 1]);
//            rowToActionDataId_pri[indexTmp] = movedActionIdTmp;
//            //for this actionId update the row value
//            actionDataIdToRow_pri[movedActionIdTmp] = indexTmp;
//            indexTmp = indexTmp - 1;
//        }
//        //replace the destination with the source (old destination has been moved +1 index)
//        rowToActionDataId_pri[destinationRow_par_con] = sourceDataIdBackup;
//        actionDataIdToRow_pri[sourceDataIdBackup] = destinationRow_par_con;
//    }
//    else
//    {
//        //the source is further up than destination, so move all the rows from the source to destination-1 and then replace destination
//        //to lose only one value, start from the source and move "down"

//        //back source id since it will be replaced by the first moved row (the one below)
//        auto sourceDataIdBackup(rowToActionDataId_pri.at(sourceRow_par_con));

//        auto indexTmp(sourceRow_par_con);
//        while (indexTmp < destinationRow_par_con)
//        {
//            //replace the row with the one below, current row = index, row below = index +1
//            auto movedActionIdTmp(rowToActionDataId_pri[indexTmp + 1]);
//            rowToActionDataId_pri[indexTmp] = movedActionIdTmp;
//            //for this actionId update the row value
//            actionDataIdToRow_pri[movedActionIdTmp] = indexTmp;
//            indexTmp = indexTmp + 1;
//        }
//        //replace the destination with the source (old destination has been moved -1 index)
//        rowToActionDataId_pri[destinationRow_par_con] = sourceDataIdBackup;
//        actionDataIdToRow_pri[sourceDataIdBackup] = destinationRow_par_con;

//    }
//}


bool actionDataController_c::insertActionData_f(
        const actionData_c& obj_par_con
        , const int row_par_con)
{
    bool resultTmp(false);
    while (true)
    {
        //invalid index
        if (row_par_con < 0 or row_par_con > size_f())
        {
            break;
        }

        //insert actionId - actionData object mapping
        actionDataIdToActionDataUMap_pri.emplace(obj_par_con.id_f(), obj_par_con);

        //new row goes in the last position
        if (row_par_con == size_f())
        {
            //insert the mappings (since it's the last row no need to touch anything else)
            actionDataIdToRow_pri.emplace(obj_par_con.id_f(), row_par_con);
            rowToActionDataId_pri.emplace(row_par_con, obj_par_con.id_f());
            resultTmp = true;
            break;
        }

        //"else"
        //new row goes in between existing ones (moving them one position and replacing one)
        //create one last row
        auto lastRowIndexTmp(size_f());
        {
            //get the previous to last
            auto movedActionIdTmp(rowToActionDataId_pri.at(lastRowIndexTmp - 1));
            //insert the actionId from the previous to last into the last
            rowToActionDataId_pri.emplace(lastRowIndexTmp, movedActionIdTmp);
            //update the actionId - row mapping
            actionDataIdToRow_pri.at(movedActionIdTmp) = lastRowIndexTmp;

            lastRowIndexTmp = lastRowIndexTmp - 1;
        }
        //move all the rows from the last to the target row, to row+1 (to make space for the new one)
        while (row_par_con < lastRowIndexTmp)
        {
            //qInfo() << "newRowIndexTmp " << newRowIndexTmp << endl;
            //qInfo() << "row_par_con " << row_par_con << endl;
            //get the actionId of the previous row
            auto movedActionIdTmp(rowToActionDataId_pri.at(lastRowIndexTmp - 1));
            //move the mappings to "row+1"
            //insert the actionId to the next row
            rowToActionDataId_pri.at(lastRowIndexTmp) = movedActionIdTmp;
            //update the actionId - row mapping with the next row
            actionDataIdToRow_pri.at(movedActionIdTmp) = lastRowIndexTmp;

            lastRowIndexTmp = lastRowIndexTmp - 1;
        }

        //since all the rows (mappings), target and below, have been moved, it's only necessary to insert
        //the new mapping in the desired row
        actionDataIdToRow_pri.emplace(obj_par_con.id_f(), row_par_con);
        rowToActionDataId_pri.at(row_par_con) = obj_par_con.id_f();
        resultTmp = true;

        break;
    }
    return resultTmp;
}


int actionDataController_c::actionDataIdToRow_f(const int_fast64_t actionDataId_par_con) const
{
    int resultTmp(-1);
    auto findResult(actionDataIdToRow_pri.find(actionDataId_par_con));
    if (findResult != actionDataIdToRow_pri.end())
    {
        resultTmp = findResult->second;
    }
    return resultTmp;
}

int_fast64_t actionDataController_c::rowToActionDataId_f(const int row_par_con) const
{
    int_fast64_t result(-1);
    auto findResult(rowToActionDataId_pri.find(row_par_con));
    if (findResult != rowToActionDataId_pri.end())
    {
        result = findResult->second;
    }
    return result;
}

int_fast32_t actionDataController_c::size_f() const
{
    return rowToActionDataId_pri.size();
}

std::pair<actionData_c&, bool> actionDataController_c::actionData_f(const int_fast64_t actionDataId_par_con)
{
    bool foundTmp(false);
    auto findResult(actionDataIdToActionDataUMap_pri.find(actionDataId_par_con));
    if (findResult != actionDataIdToActionDataUMap_pri.end())
    {
        foundTmp = true;
    }
    return {findResult->second, foundTmp};
}



void actionDataController_c::clearAllActionData_f()
{
    actionDataIdToRow_pri.clear();
    rowToActionDataId_pri.clear();
    actionDataIdToActionDataUMap_pri.clear();
}

//void actionDataController_c::runActionsDataId_f(const std::vector<int_fast32_t>& actionIds_par_con)
//{
//    for (const int_fast32_t actionId_ite_con : actionIds_par_con)
//    {
//        actionData_c& actionDataTmp(actionData_f(actionId_ite_con).first);
//        actionDataTmp.createExecution_f();
//    }

//}

//void actionDataController_c::runActionsFromRows_f(const std::vector<int>& rows_par_con)
//{
//    std::vector<int_fast32_t> dataIdsTmp;
//    dataIdsTmp.reserve(rows_par_con.size());
//    for (const int row_ite_con : rows_par_con)
//    {
//        dataIdsTmp.emplace_back(rowToActionDataId_f(row_ite_con));
//    }
//    runActionsDataId_f(dataIdsTmp);
//}

bool actionDataController_c::removeActionDataUsingRow_f(
        const int row_par_con
        , const int_fast64_t actionDataId_par_con)
{
    bool resultTmp(false);
    while (true)
    {
        //invalid index
        if (row_par_con < 0 or row_par_con >= size_f())
        {
            break;
        }

        int_fast32_t actionIdToRemoveTmp(actionDataId_par_con);
        //if it's not set, get it
        if (actionIdToRemoveTmp == 0)
        {
            actionIdToRemoveTmp = rowToActionDataId_f(row_par_con);
        }

        //removing the last row
        if (row_par_con == (size_f() - 1))
        {
            rowToActionDataId_pri.erase(row_par_con);
            actionDataIdToRow_pri.erase(actionIdToRemoveTmp);
            actionDataIdToActionDataUMap_pri.erase(actionIdToRemoveTmp);
            resultTmp = true;
            break;
        }

        //"else"
        //removing a row with one or more rows below
        //move all the rows below the removed one to "row-1" (replacing in cascade)
        auto removedRowIndexTmp(row_par_con);
        //move all the rows from the "removed row+1" to row-1 (replacing the removed one)
        while ((removedRowIndexTmp + 1) < size_f())
        {
            //qInfo() << "removedRowIndexTmp " << removedRowIndexTmp << endl;
            //qInfo() << "size_f() " << size_f() << endl;
            //get the actionId of the next row
            auto movedActionIdTmp(rowToActionDataId_pri.at(removedRowIndexTmp + 1));
            //move the mappings to the row being replaced
            //replace the actionId with the one from the next row
            rowToActionDataId_pri.at(removedRowIndexTmp) = movedActionIdTmp;
            //replace the actionId - row mapping with the next row
            actionDataIdToRow_pri.at(movedActionIdTmp) = removedRowIndexTmp;

            removedRowIndexTmp = removedRowIndexTmp + 1;
        }

        //remove the last "row", since it has been moved (but the last one doesn't get replaced by the next one)
        rowToActionDataId_pri.erase(removedRowIndexTmp);
        //remove the mapping actionId - row mapping of the actionId being deleted
        actionDataIdToRow_pri.erase(actionIdToRemoveTmp);
        //remove actionId - object mapping (removing the object from the data "container")
        actionDataIdToActionDataUMap_pri.erase(actionIdToRemoveTmp);
        resultTmp = true;
        break;
    }

    return resultTmp;
}

bool actionDataController_c::removeActionDataUsingId_f(const int_fast64_t actionDataId_par_con)
{
    bool resultTmp(false);
    auto rowFindResult(actionDataIdToRow_f(actionDataId_par_con));
    if (rowFindResult != -1)
    {
        resultTmp = removeActionDataUsingRow_f(rowFindResult);
    }
    return resultTmp;
}

actionDataController_c& dataHub_f()
{
    static actionDataController_c actionData_sta;
    return actionData_sta;
}

//////////////////
//actionDataRoot_c
//////////////////

std::vector<actionData_c> actionDataRoot_c::actionDataVector_f() const
{
    return actionDataVector_pri;
}

void actionDataRoot_c::moveToDataHub_f()
{
    //take in account existing count when inserting more rows
    int_fast32_t rowCountTmp(dataHub_f().size_f());
    for (const actionData_c& item_ite_con : actionDataVector_pri)
    {
        dataHub_f().insertActionData_f(item_ite_con, rowCountTmp);
        rowCountTmp = rowCountTmp + 1;
    }
    actionDataVector_pri.clear();
}

actionDataRoot_c::actionDataRoot_c(const std::vector<actionData_c>& actionDataVector_par_con)
: actionDataVector_pri(actionDataVector_par_con)
{}


void actionDataRoot_c::write_f(QJsonObject& json) const
{
    QJsonArray actionArrayTmp;
    for (const actionData_c& actionData_ite_con : actionDataVector_pri)
    {
        QJsonObject jsonObjectTmp;
        actionData_ite_con.write_f(jsonObjectTmp);
        actionArrayTmp.append(jsonObjectTmp);
    }
    json["actions"] = actionArrayTmp;
}

void actionDataRoot_c::read_f(const QJsonObject& json)
{
    QJsonArray arrayTmp(json["actions"].toArray());
    actionDataVector_pri.reserve(arrayTmp.size());
    for (const auto& item_ite_con : arrayTmp)
    {
        QJsonObject actionDataJsonObject = item_ite_con.toObject();
        actionData_c actionDataTmp;
        actionDataTmp.read_f(actionDataJsonObject);
        actionDataVector_pri.emplace_back(actionDataTmp);
    }
}


