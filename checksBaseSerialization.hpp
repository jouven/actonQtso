#ifndef ACTONQTSO_CHECKSBASESERIALIZATION_HPP
#define ACTONQTSO_CHECKSBASESERIALIZATION_HPP

#include "crossPlatformMacros.hpp"

class checksDataHub_c;
class QJsonObject;
class textCompilation_c;

extern EXPIMP_ACTONQTSO void copyFromChecksDataHubAndSerialize_f(
        const checksDataHub_c& checkDataHub_par_con
        , QJsonObject& json_ref_par
);
extern EXPIMP_ACTONQTSO void deserializeAndCopyToChecksDataHub_f(
        const QJsonObject &json_par_con
        , checksDataHub_c& checkDataHub_par_ref
        //validates the actions objects (and anything nested that can be validated)
        //and only loads the valid ones in the actonHub object
        , const bool loadOnlyValid_par_con
        //error text compilation to know why and which objects aren't valid
        , textCompilation_c* errors_par = nullptr);


#endif // ACTONQTSO_CHECKSBASESERIALIZATION_HPP
