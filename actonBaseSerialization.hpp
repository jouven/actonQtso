#ifndef ACTONQTSO_ACTONBASESERIALIZATION_HPP
#define ACTONQTSO_ACTONBASESERIALIZATION_HPP

#include "crossPlatformMacros.hpp"

class QJsonObject;
class textCompilation_c;
class actonDataHub_c;

extern EXPIMP_ACTONQTSO QJsonObject serializeActonDataHub_f(const actonDataHub_c* const actonDataHubPtr_par_con);
//actions are appended to the existing ones, execution options are replaced, stringParser stringTrigger/s are added or replaced if the string trigger has the same key
extern EXPIMP_ACTONQTSO void deserializeActonDataHub_f(
        const QJsonObject &json_par
        , actonDataHub_c* const actonDataHubPtr_par
        //validates the actions objects (and anything nested that can be validated)
        //and only loads the valid ones in the actonHub object
        , const bool loadOnlyValid_par_con
        //error text compilation to know why and which objects aren't valid
        , textCompilation_c* errors_par = nullptr);


#endif // ACTONQTSO_ACTONBASESERIALIZATION_HPP
