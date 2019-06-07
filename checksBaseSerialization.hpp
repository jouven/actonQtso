#ifndef ACTONQTSO_CHECKSBASESERIALIZATION_HPP
#define ACTONQTSO_CHECKSBASESERIALIZATION_HPP

#include "crossPlatformMacros.hpp"

class checksDataHub_c;
class QJsonObject;

extern EXPIMP_ACTONQTSO void copyFromChecksDataHubAndSerialize_f(
        const checksDataHub_c& checkDataHub_par_con
        , QJsonObject& json_ref_par
);
extern EXPIMP_ACTONQTSO void deserializeAndCopyToChecksDataHub_f(
        const QJsonObject &json_par_con
        , checksDataHub_c& checkDataHub_par_ref
);

#endif // ACTONQTSO_CHECKSBASESERIALIZATION_HPP
