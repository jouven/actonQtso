#ifndef ACTONQTSO_ACTONBASESERIALIZATION_HPP
#define ACTONQTSO_ACTONBASESERIALIZATION_HPP

#include <QJsonObject>

//actonDataHub_c could be serialized/deserialized in many ways,
//this is the one used for the save/load "acton" files

extern QJsonObject copyFromActonDataHubAndSerialize_f();
extern void deserializeAndCopyToActonDataHub_f(const QJsonObject &json);


#endif // ACTONQTSO_ACTONBASESERIALIZATION_HPP
