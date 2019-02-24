#ifndef LOGITEM_H
#define LOGITEM_H


#include <QMetaType>
#include <QString>

enum class LoggerVerbosity: uint32_t {
    INFO,
    WARN,
    FATAL,
    VERBOSE,
};

Q_DECLARE_METATYPE(LoggerVerbosity);


//QString verbosity_to_string(const LoggerVerbosity& verbosity) {
//    switch (verbosity) {
//        case LoggerVerbosity::INFO: {
//            return "INFO";
//        }
//        case LoggerVerbosity::WARN: {
//            return "WARN";
//        }
//        case LoggerVerbosity::FATAL: {
//            return "FATAL";
//        }
//        case LoggerVerbosity::VERBOSE: {
//            return "VERBOSE";
//        }
//    }
//}


struct LogItem {
    LoggerVerbosity verbosity;

    int64_t time;


    int32_t thread_id;

    int32_t line;

    QString tag;
    QString function_str;
    QString filename_str;
    QString msg_str;

    //u64 function_length;
    //u64 filename_length;
    //u64 msg_length;
    //
    //const char* function;
    //const char* filename;
    //const char* msg;

    LogItem();
    LogItem(const LogItem& other);
    ~LogItem();

};

Q_DECLARE_METATYPE(LogItem);

enum TABLE_COLUMNS {
    TIME = 0,
    VERBOSITY,
    TAG,

    THREAD_ID,
    FILENAME,
    FUNCTION,
    LINE_NUMBER,

    MESSAGE,
    COLUMN_COUNT
};


#endif // LOGITEM_H
