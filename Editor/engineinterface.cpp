#include "engineinterface.h"
#include <QtEndian>


EngineInterface::EngineInterface() {

}

EngineInterface::~EngineInterface() {

}

void EngineInterface::init_engine_interface(QMainWindow* main_window, QWidget* game_widget, QVBoxLayout* game_layout,
                                            DockConsole* console, DockSceneHierarchy* scene_hierarchy) {

    this->main_window = main_window;
    this->game_widget = game_widget;
    this->game_layout = game_layout;

    this->console = console;
    this->scene_hierarchy = scene_hierarchy;

    // Create a 4k buffer to hold data about incoming commands
    socket_buffer = QByteArray(4096, 0);

    engine_socket = new QTcpSocket(main_window);



    QObject::connect(engine_socket, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(engine_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(engine_socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    QObject::connect(engine_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    engine_socket->connectToHost("127.0.0.1", 27016);
    if(!engine_socket->waitForConnected(5000)) {
        qDebug() << "Error: " << engine_socket->errorString();
    }
}

void EngineInterface::destroy_engine_interface() {
    if (engine_socket != nullptr) {
        cmd_request_engine_disconnect();
        //engine_socket->disconnectFromHost();
    }
}


template<typename T>
char* write(char* buf, T value) {
    *(T*)buf = qToBigEndian<T>(value);
    return buf + sizeof(T);
}

template<typename T>
char* read(char* buf, T* dest) {
    *dest = qFromBigEndian<T>(*(T*)buf);
    return buf + sizeof(T);
}


void EngineInterface::connected()
{
    qDebug() << "connected...";
    cmd_request_engine_connect();
	

}

void EngineInterface::disconnected()
{
    qDebug() << "disconnected...";
}

void EngineInterface::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void EngineInterface::readyRead()
{
    // read the data from the socket
    //qDebug() << engine_socket->readAll();

    QByteArray bytes = engine_socket->readAll();
    // NOTE: with tcp sockets, there is no one to one send/recv. so we'll need to keep track of how many bytes read
    // Example if we send 100 bytes at one time, we might recv it in one chunk of 100 bytes, or 10 chunks of 10 bytes, or 100 chunks of 1, etc..
    // we'll need to handle this here
    int length = bytes.size();
    char* data = (char*)bytes.data();


    char* buf = data;


    // deserialze command header
    EditorCommandHeader header;
    char magic[4];
    memcpy(magic, buf, 4);
    buf += 4;
    buf = read<>(buf, &header.version);
    buf = read<>(buf, &header.message_size);

    // deserialze command body
    EditorCommand command;
    int32_t icommand_type;
    int32_t icommand_undo_type;
    buf = read<>(buf, &icommand_type);
    buf = read<>(buf, &icommand_undo_type);
    command.type = (EditorCommandType)icommand_type;
    command.undo_type = (EditorCommandType)icommand_undo_type;

    buf = read<>(buf, &command.buffer_size);

    command.buffer = (char*)calloc(1, command.buffer_size);
    memcpy(command.buffer, buf, command.buffer_size);

    qDebug() << "magic:" << magic
             << ",version " << header.version
             << ",msg_len " << header.message_size
             << ",msg_type " << (int32_t)command.type
             << ",msg_undo_type " << (int32_t)command.undo_type
             << ",buffer size" << command.buffer_size;

    recv_command_from_engine(command);

    free(command.buffer);
}

void EngineInterface::recv_command_from_engine(const EditorCommand& command) {

    switch (command.type) {

        case EditorCommandType::NONE: {
            break;
        }
        case EditorCommandType::ENGINE_CONNECT: {
            cmd_respond_engine_connect(command);
            break;
        }
        case EditorCommandType::ENGINE_DISCONNECT: {
            cmd_respond_engine_disconnect(command);
            break;
        }
        case EditorCommandType::ENGINE_DATA: {
            cmd_respond_engine_data(command);
            break;
        }
        case EditorCommandType::SEND_LOG_ITEM: {
            cmd_respond_send_log_item(command);
            break;
        }
        case EditorCommandType::EDITOR_WINDOW_FOCUS_CHANGE: {
            cmd_respond_editor_window_focus_change(command);
            break;
        }
    }
}


void EngineInterface::send_command_to_engine(const EditorCommand& command) {
    EditorCommandHeader header;
    header.version = EDITOR_COMMAND_VERSION;

    // NOTE: we explicity define the message size here. Using sizeof(command) works
    // but in cases the compiler will put padding in cases when we add new data to the command struct
    // better to explicity define the header size.
    header.message_size =
            sizeof(command.type) // type
            + sizeof(command.undo_type) // undo type
            + sizeof(command.buffer_size)
            + command.buffer_size;


    // NOTE: we explicity define the header size here. Using sizeof(header) works
    // but in cases the compiler will put padding in cases when we add new data to the header struct
    // better to explicity define the header size.
    size_t header_size =
            + sizeof(header.magic)
            + sizeof(header.version)
            + sizeof(header.message_size);

    size_t msg_size = header.message_size;
    size_t buf_size = header_size + msg_size;





    char* buf_start = socket_buffer.data();
    //char* buf_start = (char*)stack_alloc(&editor->stack, buf_size, 1);
    char* buf = buf_start;


    // Write header
    char magic[MAGIC_BYTES_SIZE] = MAGIC_BYTES;
    memcpy(buf, magic, MAGIC_BYTES_SIZE);
    buf += MAGIC_BYTES_SIZE;

    buf = write<int32_t>(buf, header.version);
    buf = write<uint64_t>(buf, (uint64_t)header.message_size);

    // Write message
    // NOTE: since we are using enum class/struct with a specified type of s32, this is safe
    // without this, enums integral types are implementation defined.
    buf = write<int32_t>(buf, (int32_t)command.type);
    buf = write<int32_t>(buf, (int32_t)command.undo_type);
    buf = write<uint64_t>(buf, (uint64_t)command.buffer_size);
    memcpy(buf, command.buffer, command.buffer_size);
    engine_socket->write(buf_start, buf_size);
}

void EngineInterface::cmd_request_engine_connect() {
     EditorCommand command;
     command.type = EditorCommandType::ENGINE_CONNECT;
     command.undo_type = EditorCommandType::NONE;
     command.buffer_size = 0;
     command.buffer = nullptr;

     send_command_to_engine(command);
}

void EngineInterface::cmd_respond_engine_connect(const EditorCommand& command) {
    uint64_t hwnd_int;

    read<>(command.buffer, &hwnd_int);

    qDebug() << "hwnd" << hwnd_int;
    WId external_winid = (WId)hwnd_int;
    engine_window = QWindow::fromWinId(external_winid);


    Qt::WindowFlags flags =
              engine_window ->flags()
            | Qt::Widget
            | Qt::FramelessWindowHint
            | Qt::ForeignWindow
            | Qt::WindowStaysOnTopHint
            | Qt::BypassWindowManagerHint
            | Qt::BypassGraphicsProxyWidget
            ;

    engine_widget = QWidget::createWindowContainer(engine_window , game_layout->widget(), flags);


    engine_widget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    auto states =
            Qt::WindowFullScreen
            | Qt::WindowMaximized
                //Qt::WindowNoState
            //Qt::WindowActive
            ;

    engine_widget->setWindowState(states);
    game_layout->addWidget(engine_widget, 1);
    engine_widget->installEventFilter(main_window);


	cmd_request_engine_data();
}

void EngineInterface::cmd_request_engine_disconnect() {
    EditorCommand command;
    command.type = EditorCommandType::ENGINE_DISCONNECT;
    command.undo_type = EditorCommandType::NONE;
    command.buffer_size = 0;
    command.buffer = nullptr;

    send_command_to_engine(command);

}

void EngineInterface::cmd_respond_engine_disconnect(const EditorCommand& command) {

}

void EngineInterface::cmd_request_engine_data() {
    EditorCommand command;
    command.type = EditorCommandType::ENGINE_DATA;
    command.undo_type = EditorCommandType::NONE;
    command.buffer_size = 0;
    command.buffer = nullptr;
    send_command_to_engine(command);
}

void EngineInterface::cmd_respond_engine_data(const EditorCommand& command) {
    uint64_t entity_count;

    char* buffer = command.buffer;
    buffer = read<>(command.buffer, &entity_count);

    qDebug() << "entity count" << entity_count;
    for (uint64_t i = 0; i < entity_count; i++) {
        uint64_t entity_id;
        uint64_t parent_entity_id;
        uint64_t name_length;
        QString name;
        uint64_t child_count;

        buffer = read<>(buffer, &entity_id);
        buffer = read<>(buffer, &name_length);
        name = QString::fromLocal8Bit(buffer, name_length);
		buffer += name_length;
        buffer = read<>(buffer, &parent_entity_id);
        buffer = read<>(buffer, &child_count);

        qDebug() << "Entity " << entity_id << " " << name << "parent " << parent_entity_id << " children " << child_count;


        scene_hierarchy->add_entity(entity_id, parent_entity_id == 0);
        scene_hierarchy->set_entity_name(entity_id, name);

        for (uint64_t j = 0; j < child_count; j++) {

            uint64_t child_entity_id;
            buffer = read<>(buffer, &child_entity_id);
            qDebug() << child_entity_id << ", ";

            scene_hierarchy->add_entity(child_entity_id, false);
            scene_hierarchy->add_entity_child(entity_id, child_entity_id);
        }


    }
}


void EngineInterface::cmd_respond_send_log_item(const EditorCommand& command) {
    LogItem item;


    uint32_t verbosity_int;
    uint64_t function_length;
    uint64_t filename_length;
    uint64_t msg_length;

    int32_t tag_length;

	char* buffer = command.buffer;
    buffer = read<>(buffer, &verbosity_int);
    buffer = read<>(buffer, &item.time);
    buffer = read<>(buffer, &item.thread_id);
    buffer = read<>(buffer, &item.line);

    item.verbosity = (LoggerVerbosity)verbosity_int;





    buffer = read<>(buffer, &tag_length);
    item.tag = QString::fromLocal8Bit(buffer, tag_length);
    buffer += tag_length;
    
	buffer = read<>(buffer, &function_length);
    item.function_str = QString::fromLocal8Bit(buffer, function_length);
	buffer += function_length;
    

	buffer = read<>(buffer, &filename_length);
    item.filename_str = QString::fromLocal8Bit(buffer, filename_length);
	buffer += filename_length;

	buffer = read<>(buffer, &msg_length);
    item.msg_str = QString::fromLocal8Bit(buffer, msg_length);
	buffer += msg_length;

    console->push_log_item(item);


}

void EngineInterface::cmd_respond_editor_window_focus_change(const EditorCommand& command) {


    int32_t has_focus_int;
    bool has_focus;
    char* buffer = command.buffer;
    buffer = read<>(buffer, &has_focus_int);
    if (has_focus_int == 1) {
        // Engine window has focus
        // Editor window has no focus
        has_focus = true;


        engine_widget->setFocus();
        //engine_widget->activateWindow();



    } else {
        // Engine window has no focus
        // Editor window has focus
        has_focus = false;
        main_window->setFocus();
        //main_window->window()->activateWindow();
        //main_window->window()->setFocus();

    }


    engine_window->setMouseGrabEnabled(!has_focus);
    engine_window->setKeyboardGrabEnabled(!has_focus);

    qDebug() << "cmd window focus changed " << has_focus;


}






































