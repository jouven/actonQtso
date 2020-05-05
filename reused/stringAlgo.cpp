#include "stringAlgo.hpp"

uint_fast64_t replaceSubString_f(QString& target_par_con, const QString& oldValue_par_con, const QString& newValue_par_con)
{
    uint_fast64_t resultTmp(0);
    int_fast64_t indexOfTmp(target_par_con.indexOf(oldValue_par_con));
    while (indexOfTmp not_eq -1)
    {
        target_par_con.replace(indexOfTmp, oldValue_par_con.size(), newValue_par_con);
        resultTmp = resultTmp + 1;
        indexOfTmp = target_par_con.indexOf(oldValue_par_con);
    }
    return resultTmp;
}

uint_fast64_t vectorQStringCountSubString_f(
        const QString& valueToFind_par_con
        , const std::vector<QString>& stringsToSearch_par_con
        , const bool existOnly_par_con)
{
    uint_fast64_t resultTmp(0);
    for (const QString& string_ite_con : stringsToSearch_par_con)
    {
        int_fast64_t indexOfTmp(string_ite_con.indexOf(valueToFind_par_con));
        while (indexOfTmp not_eq -1)
        {
            resultTmp = resultTmp + 1;
            if (existOnly_par_con and resultTmp > 0)
            {
                break;
            }
            indexOfTmp = string_ite_con.indexOf(valueToFind_par_con, indexOfTmp + 1);
        }
        if (existOnly_par_con and resultTmp > 0)
        {
            break;
        }
    }
    return resultTmp;
}

uint_fast64_t qStringListCountSubString_f(
        const QString& valueToFind_par_con
        , const QStringList& stringsToSearch_par_con
        , const bool existOnly_par_con)
{
    uint_fast64_t resultTmp(0);
    for (const QString& string_ite_con : stringsToSearch_par_con)
    {
        int_fast64_t indexOfTmp(string_ite_con.indexOf(valueToFind_par_con));
        while (indexOfTmp not_eq -1)
        {
            resultTmp = resultTmp + 1;
            if (existOnly_par_con and resultTmp > 0)
            {
                break;
            }
            indexOfTmp = string_ite_con.indexOf(valueToFind_par_con, indexOfTmp + 1);
        }
        if (existOnly_par_con and resultTmp > 0)
        {
            break;
        }
    }
    return resultTmp;
}

uint_fast64_t replaceSubString_f(std::vector<QString>& target_par_con, const QString& oldValue_par_con, const QString& newValue_par_con)
{
    uint_fast64_t resultTmp(0);
    for (QString& string_ite : target_par_con)
    {
        replaceSubString_f(string_ite, oldValue_par_con, newValue_par_con);
    }
    return resultTmp;
}

uint_fast64_t replaceSubString_f(QStringList& target_par_con, const QString& oldValue_par_con, const QString& newValue_par_con)
{
    uint_fast64_t resultTmp(0);
    for (QString& string_ite : target_par_con)
    {
        replaceSubString_f(string_ite, oldValue_par_con, newValue_par_con);
    }
    return resultTmp;
}
