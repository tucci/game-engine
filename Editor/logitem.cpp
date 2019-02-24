#include "logitem.h"

LogItem::LogItem() {

}

LogItem::LogItem(const LogItem& other) {
    verbosity = other.verbosity;
    time = other.time;
    tag = other.tag;
    thread_id = other.thread_id;
    line = other.line;

    function_str = QString(other.function_str);
    filename_str = QString(other.filename_str);
    msg_str = QString(other.msg_str);
}

LogItem::~LogItem() {

}
