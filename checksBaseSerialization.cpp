#include "checksBaseSerialization.hpp"

#include "checkData.hpp"
#include "checksDataHub.hpp"
#include "checkMappings/checkStrMapping.hpp"

#include <QJsonObject>
#include <QJsonArray>

#include <vector>

void copyFromChecksDataHubAndSerialize_f(
        const checksDataHub_c& checkDataHub_par_con
        , QJsonObject& json_ref_par
)
{
    if (checkDataHub_par_con.size_f() > 0)
    {
        std::vector<const check_c*> checkVectorTmp;
        checkVectorTmp.reserve(checkDataHub_par_con.size_f());
        for (int_fast32_t index_ite = 0, l = checkDataHub_par_con.size_f(); index_ite < l; ++index_ite)
        {
            auto checkDataIdTmp(checkDataHub_par_con.rowToCheckDataId_f(index_ite));
            checkVectorTmp.emplace_back(checkDataHub_par_con.check_ptr_f(checkDataIdTmp));
        }

        QJsonArray checkArrayTmp;
        for (const check_c* const check_ite_con : checkVectorTmp)
        {
            QJsonObject jsonObjectTmp;
            check_ite_con->write_f(jsonObjectTmp);
            checkArrayTmp.append(jsonObjectTmp);
        }
        json_ref_par["checks"] = checkArrayTmp;
    }
    else
    {
    }
}

void deserializeAndCopyToChecksDataHub_f(
        const QJsonObject& json_par_con
        , checksDataHub_c& checkDataHub_par_ref
)
{
    if (json_par_con["checks"].isUndefined())
    {
    }
    else
    {
        QJsonArray arrayTmp(json_par_con["checks"].toArray());
        std::vector<check_c*> checkVectorTmp;
        checkVectorTmp.reserve(arrayTmp.size());
        for (const auto& item_ite_con : arrayTmp)
        {
            QJsonObject checkDataJsonObject(item_ite_con.toObject());
            checkType_ec checkTypeTmp(strToCheckTypeMap_glo_sta_con.value(checkDataJsonObject["type"].toString().toLower()));

            check_c* checkDataPtrTmp(check_c::readCreateDerived_f(checkTypeTmp));
            if (checkDataPtrTmp not_eq nullptr)
            {
                checkDataPtrTmp->read_f(checkDataJsonObject);
                checkVectorTmp.emplace_back(checkDataPtrTmp);
            }
        }

        //take in account existing count when inserting more rows
        auto rowCountTmp(checkDataHub_par_ref.size_f());
        for (check_c* item_ite_con : checkVectorTmp)
        {
            checkDataHub_par_ref.insertCheck_f(item_ite_con, rowCountTmp);
            rowCountTmp = rowCountTmp + 1;
        }
    }
}
