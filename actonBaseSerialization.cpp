#include "actonBaseSerialization.hpp"

#include "actonDataHub.hpp"
#include "actionData.hpp"
#include "actonExecutionOptions.hpp"
#include "actionMappings/actionStrMapping.hpp"

#include <QJsonArray>
#include <QJsonObject>

#include <vector>

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

QJsonObject copyFromActonDataHubAndSerialize_f()
{
    QJsonObject jsonTmp;

    if (actonDataHub_ptr_ext->size_f() > 0)
    {
        std::vector<const action_c*> actionVectorTmp;
        actionVectorTmp.reserve(actonDataHub_ptr_ext->size_f());
        for (int_fast32_t index_ite = 0, l = actonDataHub_ptr_ext->size_f(); index_ite < l; ++index_ite)
        {
            auto actionDataIdTmp(actonDataHub_ptr_ext->rowToActionDataId_f(index_ite));
            actionVectorTmp.emplace_back(actonDataHub_ptr_ext->action_ptr_f(actionDataIdTmp));
        }

        QJsonArray actionArrayTmp;
        for (const action_c* const action_ite_con : actionVectorTmp)
        {
            QJsonObject jsonObjectTmp;
            action_ite_con->write_f(jsonObjectTmp);
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
        std::vector<action_c*> actionVectorTmp;
        QJsonArray arrayTmp(json["actions"].toArray());
        actionVectorTmp.reserve(arrayTmp.size());
        for (const auto& item_ite_con : arrayTmp)
        {
            QJsonObject actionDataJsonObject(item_ite_con.toObject());
            actionType_ec actionTypeTmp(strToActionTypeMap_ext_con.value(actionDataJsonObject["type"].toString().toLower()));
#ifdef DEBUGJOUVEN
            //qDebug() << "actionData_c* actionDataPtrTmp(actionData_c::readCreateDerived_f(actionTypeTmp));" << endl;
#endif
            action_c* actionPtrTmp(action_c::readCreateDerived_f(actionTypeTmp));

            if (actionPtrTmp not_eq nullptr)
            {
#ifdef DEBUGJOUVEN
                //qDebug() << "actionDataPtrTmp->read_f(actionDataJsonObject);" << endl;
#endif
                actionPtrTmp->read_f(actionDataJsonObject);
                actionVectorTmp.emplace_back(actionPtrTmp);
#ifdef DEBUGJOUVEN
                //qDebug() << "after actionVectorTmp.emplace_back(std::move(actionDataTmp));" << endl;
#endif
            }
        }

        //take in account existing count when inserting more rows
        auto rowCountTmp(actonDataHub_ptr_ext->size_f());
#ifdef DEBUGJOUVEN
        //qDebug() << "actonBaseSerialization_c::moveToDataHub_f() before loop" << endl;
#endif
        for (action_c* item_ite_con : actionVectorTmp)
        {
            actonDataHub_ptr_ext->insertActionData_f(item_ite_con, rowCountTmp);
            rowCountTmp = rowCountTmp + 1;
        }
    }

    actonDataHub_ptr_ext->executionOptions_f().read_f(json["executeOptions"].toObject());
}
