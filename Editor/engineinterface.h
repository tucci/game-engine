#ifndef ENGINEINTERFACE_H
#define ENGINEINTERFACE_H

#include <QWindow>
#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QTcpSocket>


#include "dockscenehierarchy.h"
#include "dockconsole.h"

#include <stdint.h>

#define EDITOR_COMMAND_VERSION 1

#define MAGIC_BYTES_SIZE 4
// 1 2 3 4 for big endian and
// 4 3 2 1 for little endian
#define MAGIC_BYTES {'1', '2', '3', '4'}

enum struct EditorCommandType : int32_t {
    NONE = 0,
    ENGINE_CONNECT = 1,
    ENGINE_DISCONNECT = 2,
    ENGINE_DATA = 3,
    SEND_LOG_ITEM = 4,
    EDITOR_WINDOW_FOCUS_CHANGE = 5
};


struct EditorCommandHeader {
    // Header
    // The magic bytes tells the parser what type of what this packet is for
    // also can be used to check endianness
    char magic[MAGIC_BYTES_SIZE];

    int32_t version;
    //s32 compression;
    size_t message_size;
};

struct EditorCommand {

    // Message
    EditorCommandType type;
    EditorCommandType undo_type;
    size_t buffer_size;
    char* buffer;
};


class EngineInterface : public QObject {
       Q_OBJECT
public:
    EngineInterface();
    ~EngineInterface();
    void init_engine_interface(QMainWindow* main_window, QWidget* game_widget, QVBoxLayout* game_layout,
                               DockConsole* console,
                               DockSceneHierarchy* scene_hierarchy);
    void destroy_engine_interface();
private:

    QMainWindow* main_window;
    QWindow* engine_window;
    QWidget* engine_widget;
    QWidget* game_widget;
    QVBoxLayout* game_layout;
    QByteArray socket_buffer;
    QTcpSocket* engine_socket;

    DockSceneHierarchy* scene_hierarchy;
    DockConsole* console;

    void recv_command_from_engine(const EditorCommand& command);
    void send_command_to_engine(const EditorCommand& command);

    void cmd_request_engine_connect();
    void cmd_respond_engine_connect(const EditorCommand& command);

    void cmd_request_engine_disconnect();
    void cmd_respond_engine_disconnect(const EditorCommand& command);

    void cmd_request_engine_data();
    void cmd_respond_engine_data(const EditorCommand& command);

    void cmd_respond_send_log_item(const EditorCommand& command);

    void cmd_respond_editor_window_focus_change(const EditorCommand& command);
private slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
};


template<typename T>
char* write(char* buf, T value);

template<typename T>
char* read(char* buf, T* dest);



#endif // ENGINEINTERFACE_H
