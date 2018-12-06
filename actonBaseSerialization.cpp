#include "actonBaseSerialization.hpp"

#include "actonDataHub.hpp"
#include "actionData.hpp"
#include "actonExecutionOptions.hpp"

#include <vector>

#include <QJsonArray>

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

QJsonObject copyFromActonDataHubAndSerialize_f()
{
    QJsonObject jsonTmp;

    if (actonDataHub_ptr_ext->size_f() > 0)
    {
        std::vector<actionData_c> actionVectorTmp;
        actionVectorTmp.reserve(actonDataHub_ptr_ext->size_f());
        for (int_fast32_t index_ite = 0, l = actonDataHub_ptr_ext->size_f(); index_ite < l; ++index_ite)
        {
            auto actionDataIdTmp(actonDataHub_ptr_ext->rowToActionDataId_f(index_ite));
            actionVectorTmp.emplace_back(*(actonDataHub_ptr_ext->actionData_ptr_f(actionDataIdTmp)));
        }

        QJsonArray actionArrayTmp;
        for (const actionData_c& actionData_ite_con : actionVectorTmp)
        {
            QJsonObject jsonObjectTmp;
            actionData_ite_con.write_f(jsonObjectTmp);
            actionArrayTmp.append(jsonObjectTmp);
        }
        jsonTmp["actions"] = actionArrayTmp;
    }

    {
        QJsonObject jsonObjectTmp;
        actonDataHub_ptr_ext->executionOptions_f().write_f(jsonObjectTmp);
        jsonTmp["executeOptions"] = jsonObjectTmp;
    }
    return jsonTmp;
}


void deserializeAndCopyToActonDataHub_f(const QJsonObject& json)
{
    if (json["actions"].isUndefined())
    {
    }
    else
    {
        std::vector<actionData_c> actionVectorTmp;
        QJsonArray arrayTmp(json["actions"].toArray());
        actionVectorTmp.reserve(arrayTmp.size());
        for (const auto& item_ite_con : arrayTmp)
        {
            QJsonObject actionDataJsonObject(item_ite_con.toObject());
            actionData_c actionDataTmp;
            actionDataTmp.read_f(actionDataJsonObject);
            actionVectorTmp.emplace_back(std::move(actionDataTmp));
#ifdef DEBUGJOUVEN
            //qDebug() << "after actionVectorTmp.emplace_back(std::move(actionDataTmp));" << endl;
#endif
        }

        //take in account existing count when inserting more rows
        auto rowCountTmp(actonDataHub_ptr_ext->size_f());
#ifdef DEBUGJOUVEN
        //qDebug() << "actonBaseSerialization_c::moveToDataHub_f() before loop" << endl;
#endif
        for (const actionData_c& item_ite_con : actionVectorTmp)
        {
            actonDataHub_ptr_ext->insertActionData_f(item_ite_con, rowCountTmp);
            rowCountTmp = rowCountTmp + 1;
#ifdef DEBUGJOUVEN
            //qDebug() << "actonBaseSerialization_c::moveToDataHub_f() actionDataExecutionResult_ptr_pri.isNullPtr_f() " << item_ite_con.actionDataExecutionResult_ptr_f() << endl;
#endif
        }
    }

    actonDataHub_ptr_ext->executionOptions_f().read_f(json["executeOptions"].toObject());
}
