#ifndef ACTONQTSO_SAMEFILEALGO_HPP
#define ACTONQTSO_SAMEFILEALGO_HPP

#include "baseClassQtso/baseClassQt.hpp"

#include <QObject>
#include <QString>

#include <vector>

class checkSameFile_c : public QObject, public baseClassQt_c
{
    Q_OBJECT
public:
    enum class state_ec
    {
        initial,
        running,
        stopped,
        error,
        finished
    };
private:
    std::vector<state_ec> stateQueue_pri;
    bool pleaseStop_pri = false;

    QString fileA_pri;
    QString fileB_pri;

    int_fast64_t bufferSize_pri = 1024 * 32;

    //index where fileA and fileB start being different, only set (>0) when files are different
    //it's not exact since comparing is done in blocks
    //but it's close, the maximum "error" range depends on the buffer size
    int_fast64_t indexWhereFilesAreDifferent_pri = 0;
public:
    checkSameFile_c() = delete;
    checkSameFile_c(
            const QString& fileA_par_con
            , const QString& fileB_par_con
    );

    QString fileA_f() const;
    void setFileA_f(const QString& fileA_par_con);
    QString fileB_f() const;
    void setFileB_f(const QString& fileB_par_con);
    int_fast64_t bufferSize_f() const;
    void setBufferSize_f(const int_fast64_t& bufferSize_par_con);

    bool checkIfSameFile_f();

    std::vector<state_ec> stateQueue_f() const;
    state_ec currentState_f() const;
    void stop_f();
    int_fast64_t indexWhereFilesAreDifferent_f() const;

private Q_SLOTS:
    void updateToErrorState_f();
Q_SIGNALS:
    void error_signal(const QString& error_par);
    void finished_signal();
    void running_signal();
    void stopped_signal();
};

#endif // ACTONQTSO_SAMEFILEALGO_HPP
