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

QJsonObject serializeActonDataHub_f(
        const actonDataHub_c* const actonDataHubPtr_par_con)
{
    QJsonObject jsonTmp;
    while (true)
    {
        if (actonDataHubPtr_par_con == nullptr)
        {
            break;
        }

        if (actonDataHubPtr_par_con->size_f() > 0)
        {
            std::vector<const action_c*> actionVectorTmp;
            actionVectorTmp.reserve(actonDataHubPtr_par_con->size_f());
            for (int_fast32_t index_ite = 0, l = actonDataHubPtr_par_con->size_f(); index_ite < l; ++index_ite)
            {
                auto actionDataIdTmp(actonDataHubPtr_par_con->rowToActionDataId_f(index_ite));
                actionVectorTmp.emplace_back(actonDataHubPtr_par_con->action_ptr_f(actionDataIdTmp));
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
            QJsonObject executeOptionsJsonObjectTmp;
            actonDataHubPtr_par_con->executionOptions_f().write_f(executeOptionsJsonObjectTmp);
            jsonTmp["executeOptions"] = executeOptionsJsonObjectTmp;
        }
        break;
    }
    return jsonTmp;
}


void deserializeActonDataHub_f(
        const QJsonObject& json_par
        , actonDataHub_c* const actonDataHubPtr_par
        , const bool loadOnlyValid_par_con
        , textCompilation_c* errors_par)
{
    while (true)
    {
        if (actonDataHubPtr_par == nullptr)
        {
            break;
        }
        if (json_par["actions"].isUndefined())
        {
        }
        else
        {
            std::vector<action_c*> actionVectorTmp;
            QJsonArray arrayTmp(json_par["actions"].toArray());
            actionVectorTmp.reserve(arrayTmp.size());
            for (const auto& item_ite_con : arrayTmp)
            {
                QJsonObject actionDataJsonObject(item_ite_con.toObject());
                actionType_ec actionTypeTmp(strToActionTypeMap_ext_con.value(actionDataJsonObject["type"].toString().toLower()));
#ifdef DEBUGJOUVEN
                //qDebug() << "actionData_c* actionDataPtrTmp(actionData_c::readCreateDerived_f(actionTypeTmp));" << endl;
#endif
                if (actionTypeTmp == actionType_ec::empty)
                {
                    text_c deserializeErrorTmp("Invalid action type {0}"
                            , actionDataJsonObject["type"].toString().toLower());
                    MACRO_ADDACTONQTSOLOG(deserializeErrorTmp, logItem_c::type_ec::error);
                    break;
                }

                action_c* actionPtrTmp(action_c::readCreateDerived_f(actionTypeTmp));

                if (actionPtrTmp not_eq nullptr)
                {
#ifdef DEBUGJOUVEN
                    //qDebug() << "actionDataPtrTmp->read_f(actionDataJsonObject);" << endl;
#endif
                    actionPtrTmp->read_f(actionDataJsonObject, loadOnlyValid_par_con, errors_par);
                    if (loadOnlyValid_par_con)
                    {
                        if (actionPtrTmp->isFieldsActionValid_f(errors_par))
                        {
                            actionVectorTmp.emplace_back(actionPtrTmp);
                        }
                    }
                    else
                    {
                        actionVectorTmp.emplace_back(actionPtrTmp);
                    }
#ifdef DEBUGJOUVEN
                    //qDebug() << "after actionVectorTmp.emplace_back(std::move(actionDataTmp));" << endl;
#endif
                }
                else
                {
                    text_c deserializeErrorTmp("Failed to deserialize action JSON: type {0} description {1}"
                            , actionDataJsonObject["type"].toString().toLower()
                            , actionDataJsonObject["description"].toString().toLower());
                    MACRO_ADDACTONQTSOLOG(deserializeErrorTmp, logItem_c::type_ec::error);
                }
            }

            //take in account existing count when inserting more rows
            auto rowCountTmp(actonDataHubPtr_par->size_f());
#ifdef DEBUGJOUVEN
            //qDebug() << "actonBaseSerialization_c::moveToDataHub_f() before loop" << endl;
#endif
            for (action_c* item_ite_con : actionVectorTmp)
            {
                actonDataHubPtr_par->insertActionData_f(item_ite_con, rowCountTmp);
                rowCountTmp = rowCountTmp + 1;
            }
        }

        executionOptions_c executionOptionsTmp;
        //keep the old stringParserMap value, since reading json doesn't affect the "main" object
        executionOptionsTmp.setStringParserMap_f(executionOptionsTmp.stringParserMap_f());
        executionOptionsTmp.read_f(json_par["executeOptions"].toObject());
        actonDataHubPtr_par->setExecutionOptions_f(executionOptionsTmp);
        break;
    }
}
