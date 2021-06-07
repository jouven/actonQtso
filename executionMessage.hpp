#ifndef ACTONQTSO_EXECUTIONMESSAGE_HPP
#define ACTONQTSO_EXECUTIONMESSAGE_HPP

#include "crossPlatformMacros.hpp"

#include "textQtso/text.hpp"

#include <QObject>
#include <QString>


//this is a "generic" class to store the messages of execution results (action and checks)
//in the same container
//this class inherits QObject because it will be "newed" and used through ptrs, also it's easier to destroy if destroying the parent is enough
//the objects of this class life are tied to the execution results, not the actions or checks
class EXPIMP_ACTONQTSO executionMessage_c : public QObject
{
    Q_OBJECT

    //this is a c++17 feature
    //order+reference index, static inline makes it shared among all of the classes, inline is necessary because it won't compile otherwise
    //this way there is an order and a reference for all the messages
    inline static std::atomic<int_fast64_t> atomicIndex_pri = {0};
    //because order, timestamp can't be trusted
    uint_fast64_t id_pri = 0;

    //what action/check this message is referencing
    //there is no direct reference to the action/check through a ptr because this message can outlive them (e.g. cloned action/checks from actions like folderChangeReaction)
    QString reference_pri;

    //message text
    textCompilation_c text_pri;
public:
    enum class type_ec
    {
        empty
        , information
        , warning
        , error
        , externalsdout //stdout from an external process
        , externalstderr //stderr from an external process
        //, debug ?
    };

    static EXPIMP_ACTONQTSO QString executionMessageTypeToString_f(const type_ec executionMessageType_par_con);
    static EXPIMP_ACTONQTSO type_ec stringToExecutionMessageType_f(const QString& string_par_con);
    static EXPIMP_ACTONQTSO QList<QString> executionMessageTypeStringValues_f();
private:
    type_ec type_pri = type_ec::information;

    qint64 timestamp_pri = 0;

    //this is go give the ability to "link" an object that is referencing the message so it can be fixed if a message is getting destroyed
    //this is already necessary for executionResult_c
    QObject* objThatHasExecutionMessagesPtr_pri = nullptr;
public:
    //for RAII objects
    explicit executionMessage_c(
            const QString& reference_par_con
            , const textCompilation_c& text_par_con
            , const type_ec type_par_con
            //if default value is left it will be set to the current time in the ctor
            //the time can be initialized when "x" happens or when the message is ctored
            , const uint_fast64_t timestamp_par_con = 0
    );
    //when creating object using new
    explicit executionMessage_c(
            //see objThatHasExecutionMessagesPtr_pri
            QObject* objThatHasExecutionMessagesPtr_par
            , const QString& reference_par_con
            , const textCompilation_c& text_par_con
            , const type_ec type_par_con
            //if default value is left it will be set to the current time in the ctor
            //the time can be initialized when "x" happens or when the message is ctored
            , const uint_fast64_t timestamp_par_con = 0
    );

    uint_fast64_t id_f() const;
    QString reference_f() const;
    textCompilation_c text_f() const;
    executionMessage_c::type_ec type_f() const;
    qint64 timestamp_f() const;
    QObject* objThatHasExecutionMessagesPtr_f() const;
    //when the execution result or another obj that is referenced by objThatHasExecutionMessagesPtr_pri has to be destroyed, use this to nullptr the "link"
    void unreferenceObjThatHasExecutionMessages_f();
};

#endif // ACTONQTSO_EXECUTIONMESSAGE_HPP
