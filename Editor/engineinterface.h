#ifndef ENGINEINTERFACE_H
#define ENGINEINTERFACE_H

#include <QWindow>
#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QTcpSocket>


#include "../Engine2/Shared/EditorCommands.h"

#include "dockscenehierarchy.h"
#include "dockconsole.h"

#include <stdint.h>


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
    QByteArray cmd_data_buffer;
    QTcpSocket* engine_socket;

    DockSceneHierarchy* scene_hierarchy;
    DockConsole* console;

    void recv_command_from_engine(const EditorCommand& command);
    void send_command_to_engine(const EditorCommand& command);

    void cmd_request_engine_connect();
    void cmd_respond_engine_connect(const EditorCommand& command);

    void cmd_request_engine_disconnect();
    void cmd_respond_engine_disconnect(const EditorCommand& command);

    void cmd_request_scene_hierarchy();

    void cmd_respond_engine_data(const EditorCommand& command);
    void cmd_respond_send_log_item(const EditorCommand& command);
    void cmd_respond_editor_window_focus_change(const EditorCommand& command);
    void cmd_respond_editor_new_entity(const EditorCommand& command);
    void cmd_respond_editor_delete_entities(const EditorCommand& command);
    void cmd_respond_editor_duplicate_entities(const EditorCommand& command);

private slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

    void cmd_request_editor_new_entity();
    void cmd_request_editor_delete_entity_list(const QList<uint64_t>& entites);
    void cmd_request_editor_select_entity_list(const QSet<uint64_t>& entities);
    void cmd_request_editor_duplicate_entity_list(const QList<uint64_t>& entities);
};


template<typename T>
char* write(char* buf, T value);

template<typename T>
char* read(char* buf, T* dest);



#endif // ENGINEINTERFACE_H
