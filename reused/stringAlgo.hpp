#ifndef ACTONQTSO_STRINGALGO_HPP
#define ACTONQTSO_STRINGALGO_HPP

#include "crossPlatformMacros.hpp"

#include <QString>
#include <QStringList>

#include <vector>

//returns the amount of replacements
uint_fast64_t EXPIMP_ACTONQTSO replaceSubString_f(
        QString& target_par_con
        , const QString& oldValue_par_con
        , const QString& newValue_par_con);
//I could template.... (do it if more container functions are needed)
uint_fast64_t EXPIMP_ACTONQTSO replaceSubString_f(
        std::vector<QString>& target_par_con
        , const QString& oldValue_par_con
        , const QString& newValue_par_con);
uint_fast64_t EXPIMP_ACTONQTSO replaceSubString_f(
        QStringList& target_par_con
        , const QString& oldValue_par_con
        , const QString& newValue_par_con);
//makes the above explicit functions, to prevent implitict type conversions
template <class T>
uint_fast64_t replaceSubString_f(
        T& target_par_con
        , const QString& oldValue_par_con
        , const QString& newValue_par_con) = delete;

//counts
uint_fast64_t EXPIMP_ACTONQTSO vectorQStringCountSubString_f(
        const QString& valueToFind_par_con
        , const std::vector<QString>& stringsToSearch_par_con
        //stop at the first ocurrence
        , const bool existOnly_par_con = false);
uint_fast64_t EXPIMP_ACTONQTSO qStringListCountSubString_f(
        const QString& valueToFind_par_con
        , const QStringList& stringsToSearch_par_con
        //stop at the first ocurrence
        , const bool existOnly_par_con = false);

#endif // ACTONQTSO_STRINGALGO_HPP
