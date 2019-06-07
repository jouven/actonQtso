#ifndef ACTONQTSO_ACTONBASESERIALIZATION_HPP
#define ACTONQTSO_ACTONBASESERIALIZATION_HPP

#include "crossPlatformMacros.hpp"

class QJsonObject;

//actonDataHub_c could be serialized/deserialized in many ways,
//this is the one used for the save/load "acton" files

extern EXPIMP_ACTONQTSO QJsonObject copyFromActonDataHubAndSerialize_f();
extern EXPIMP_ACTONQTSO void deserializeAndCopyToActonDataHub_f(const QJsonObject &json);


#endif // ACTONQTSO_ACTONBASESERIALIZATION_HPP
