#include "checksBaseSerialization.hpp"

#include "checkData.hpp"
#include "checksDataHub.hpp"

#include <QJsonArray>

#include <vector>

void copyFromChecksDataHubAndSerialize_f(
        const checksDataHub_c& checkDataHub_par_con
        , QJsonObject& json_ref_par
)
{
    if (checkDataHub_par_con.size_f() > 0)
    {
        std::vector<checkData_c> checkDataVectorTmp;
        checkDataVectorTmp.reserve(checkDataHub_par_con.size_f());
        for (int_fast32_t index_ite = 0, l = checkDataHub_par_con.size_f(); index_ite < l; ++index_ite)
        {
            auto checkDataIdTmp(checkDataHub_par_con.rowToCheckDataId_f(index_ite));
            checkDataVectorTmp.emplace_back(checkDataHub_par_con.checkData_f(checkDataIdTmp));
        }

        QJsonArray checkArrayTmp;
        for (const checkData_c& checkData_ite_con : checkDataVectorTmp)
        {
            QJsonObject jsonObjectTmp;
            checkData_ite_con.write_f(jsonObjectTmp);
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
        std::vector<checkData_c> checkDataVectorTmp;
        checkDataVectorTmp.reserve(arrayTmp.size());
        for (const auto& item_ite_con : arrayTmp)
        {
            QJsonObject checkDataJsonObject(item_ite_con.toObject());
            checkData_c checkDataTmp;
            checkDataTmp.read_f(checkDataJsonObject);
            checkDataVectorTmp.emplace_back(std::move(checkDataTmp));
        }

        //take in account existing count when inserting more rows
        auto rowCountTmp(checkDataHub_par_ref.size_f());
        for (const checkData_c& item_ite_con : checkDataVectorTmp)
        {
            checkDataHub_par_ref.insertCheckData_f(item_ite_con, rowCountTmp);
            rowCountTmp = rowCountTmp + 1;
        }
    }
}
