#ifndef ACTONQTSO_EXECUTIONRESULT_HPP
#define ACTONQTSO_EXECUTIONRESULT_HPP

#include "executionMessage.hpp"

#include "crossPlatformMacros.hpp"

#include <QObject>

#include <vector>
#include <unordered_set>

//this is a pure class for the other executions details classes, making it this way allows to put ptrs of the child classes
//in the same container
//the derived objects of this class, life is not tied to the execution, not the actions or checks
//see std::deque<executionResult_c*> executionResults_pri; in the actonDataHub_c class
class EXPIMP_ACTONQTSO executionResult_c : public QObject
{
    Q_OBJECT

    //this is a c++17 feature (the inline static inside class behavior)
    //order index, static inline makes it shared among all of the classes, inline is necessary because it won't compile otherwise

    //this is not necessarily here to have the executionResults in order but, like I mention in the executionMessage_c class,
    //it's to be able to identify an object or references to, case in point, in the gui program (actonQtg)
    //I need a way to know which execution result row is being updated when a signal updating a field from that row happens,
    //I don't have any kind of mapping container or data modelling, but with a unique Id the rows can be searched
    //FUTURE? do a model-view in the GUI because this is not an optimal solution, specially since the sorting can be changed
    //on the table
    inline static std::atomic<int_fast64_t> atomicIndex_pri = {0};
    uint_fast64_t id_pri = 0;

protected:
    std::vector<executionMessage_c*> messages_pro;

    bool started_pro = false;
    qint64 startTime_pro = 0;
    bool finished_pro = false;
    qint64 finishedTime_pro = 0;

    bool stoppedByUser_pro = false;

    virtual bool derivedTrySetFinished_f() = 0;
    virtual void derivedAppendMessage_f(executionMessage_c* message_par_con) = 0;

public:
    enum class type_ec
    {
        empty
        , action
        , check
    };

    static EXPIMP_ACTONQTSO QString executionResultTypeToString_f(const type_ec executionResultType_par_con);
    static EXPIMP_ACTONQTSO type_ec stringToExecutionResultType_f(const QString& string_par_con);
    static EXPIMP_ACTONQTSO QList<QString> executionResultTypeStringValues_f();

    uint_fast64_t id_f() const;
    virtual type_ec type_f() const = 0;
    virtual QString stateString_f() const = 0;

    //if a list of all the values is required, "append" checkStringValues_f and actionStringValues_f (actionStrMapping.hpp and checkStrMapping.hpp need to be revamped)
    //action/check type
    virtual QString derivedElementTypeString_f() const = 0;
    //action/check description
    virtual QString derivedElementDescription_f() const = 0;

    executionResult_c(QObject* parent_par);

    //default argument returns all types
    std::vector<executionMessage_c*> messages_f(const std::unordered_set<executionMessage_c::type_ec>& types_par_con = std::unordered_set<executionMessage_c::type_ec>()) const;
    textCompilation_c messagesTextCompilation_f(const std::unordered_set<executionMessage_c::type_ec>& types_par_con = std::unordered_set<executionMessage_c::type_ec>()) const;
    //returns the message/s text to string concatenated
    QString messagesStr_f(const std::unordered_set<executionMessage_c::type_ec>& types_par_con = std::unordered_set<executionMessage_c::type_ec>()) const;
    size_t messageCount_f() const;
    //can return nullptr when no messages
    executionMessage_c* lastMessage_f() const;

    bool hasErrorMessages_f() const;

    bool started_f() const;
    bool finished_f() const;

    bool stoppedByUser_f() const;

    //start time begins when the execution state changes from initial to executingChecks or preparing.
    //milliseconds since epoch
    qint64 startTime_f() const;
    qint64 finishedTime_f() const;


    //will only set started if started_pro == false
    void setStarted_f();

    void trySetFinished_f();
    //does a find and if found removes the message/s in messages_pro (doesn't destroy them)
    void unreferenceMessages_f(const std::vector<executionMessage_c*>& messagePtrs_par_con);
Q_SIGNALS:
    void anyChange_signal(executionResult_c* executionResult_ptr_par);

    void messageAdded_signal(executionResult_c* executionResult_ptr_par, executionMessage_c* message_par);
    void informationMessageAdded_signal(executionResult_c* executionResult_ptr_par, executionMessage_c* message_par);
    void warningMessageAdded_signal(executionResult_c* executionResult_ptr_par, executionMessage_c* message_par);
    void errorMessageAdded_signal(executionResult_c* executionResult_ptr_par, executionMessage_c* message_par);

    void started_signal(executionResult_c* executionResult_ptr_par);
    void finished_signal(executionResult_c* executionResult_ptr_par);

    void stopping_signal(executionResult_c* executionResult_ptr_par);
    void stopped_signal(executionResult_c* executionResult_ptr_par);

    void preparing_signal(executionResult_c* executionResult_ptr_par);
    void executing_signal(executionResult_c* executionResult_ptr_par);
    //void error_signal(executionResult_c* executionResult_ptr_par);

public Q_SLOTS:
    void appendMessage_f(executionMessage_c* message_par_con);
};

#endif // ACTONQTSO_EXECUTIONRESULT_HPP
