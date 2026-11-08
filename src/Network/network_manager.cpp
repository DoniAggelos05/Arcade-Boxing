#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <godot_cpp/classes/engine.hpp>
#include "network_manager.h"
#include <godot_cpp/classes/json.hpp>

using namespace godot;

void NetworkManager::_bind_methods()
{
    ADD_SIGNAL(MethodInfo("connected"));
    ADD_SIGNAL(MethodInfo("connection_closed"));
    ADD_SIGNAL(MethodInfo("disconnected"));
    ADD_SIGNAL(MethodInfo("lobbies_received", PropertyInfo(Variant::ARRAY, "list")));
    ADD_SIGNAL(MethodInfo("created_lobby",PropertyInfo(Variant::DICTIONARY, "msg")));
    ADD_SIGNAL(MethodInfo("server_error",PropertyInfo(Variant::STRING,"reason")));
    ADD_SIGNAL(MethodInfo("lobby_closed"));
    ADD_SIGNAL(MethodInfo("peer_joined",
        PropertyInfo(Variant::INT,    "id"),
        PropertyInfo(Variant::STRING, "name"),
        PropertyInfo(Variant::INT,    "character")));
    ADD_SIGNAL(MethodInfo("peer_left", PropertyInfo(Variant::INT, "id")));
    ADD_SIGNAL(MethodInfo("lock_changed",PropertyInfo(Variant::BOOL, "locked")));
    ADD_SIGNAL(MethodInfo("kicked"));
}

void NetworkManager::_ready()
{
    set_process(false);
     if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    socket.instantiate();
}

void NetworkManager::_process(double delta)
{
    if (socket.is_null()) return;
    socket->poll();
    int state = socket->get_ready_state();
    if(state == WebSocketPeer::STATE_OPEN)
    {
        while (socket->get_available_packet_count() > 0)
        {
            PackedByteArray data = socket->get_packet();
            String dataText = data.get_string_from_utf8();
            Variant parsed = JSON::parse_string(dataText);
            if (parsed.get_type() != Variant::DICTIONARY) continue;
            Dictionary msg = parsed;
            handle_message(msg);

        }
        
    }
    else if(state == WebSocketPeer::STATE_CLOSED)
    {
        if(is_connecting){
            is_connecting = false;
            emit_signal("connection_closed");
        }
        if(connected)
        {
            connected = false;
            emit_signal("disconnected");
        }
        set_process(false);

    }

    if(is_connecting)
    {
        connect_timer += delta;
        if(connect_timer > 10)
        {
            socket->close();
            is_connecting = false;
            emit_signal("connection_closed");
        }
    }
}


void NetworkManager::connect_to_serv()
{
    if(socket->get_ready_state() == WebSocketPeer::STATE_OPEN && !is_connecting && connected)
    {
        emit_signal("connected");
        return;
    }
    if(is_connecting) return;
    if(socket->get_ready_state() != WebSocketPeer::STATE_CLOSED)
    {
        socket.instantiate();
    }
    socket->connect_to_url("ws://doniaggelos.duckdns.org:8443");
    connect_timer = 0.0;
    is_connecting = true;
    set_process(true);
}


void NetworkManager::disc_from_serv()
{
    if(socket->get_ready_state() == WebSocketPeer::STATE_CLOSED) return;
    connected = false;
    is_connecting = false;
    socket->close();
}

void NetworkManager::handle_message(const Dictionary& msg)
{
    String type = msg.get("type","");

    if(type == "welcome" && is_connecting)
    {
        is_connecting = false;
        connected = true;  
        emit_signal("connected"); 
    }
    else if(type == "lobbies")
    {
        Array list = msg.get("list",Array());
        emit_signal("lobbies_received",list);
    }
    else if(type == "lobby_created" || type == "joined")
    {
        curr_lobby = msg;
        emit_signal("created_lobby",msg);
    }
    else if(type=="error")
    {
        emit_signal("server_error",msg.get("reason",""));
    }
    else if(type=="lobby_closed")
    {
        curr_lobby.clear();
        emit_signal("lobby_closed");
    }
    else if (type == "peer_joined")
    {
        int id = msg.get("id", 0);
        String name = msg.get("name", "");
        int character = msg.get("character", 0);
        emit_signal("peer_joined", id, name, character);
    }
    else if (type == "peer_left")
    {
        int id = msg.get("id", 0);
        emit_signal("peer_left", id);
    }
    else if( type == "lock_changed")
    {
        bool lock = msg.get("locked",false);
        curr_lobby["isPrivate"] = lock;
        emit_signal("lock_changed",lock);
    }
    else if(type == "kicked")
    {
        curr_lobby.clear();
        emit_signal("kicked");
    }
    
}

void NetworkManager::request_lobbies()
{
    Dictionary msg;
    msg["type"] = "list_lobbies";
    send_message(msg);
}

void NetworkManager::create_lobby(const String& lobby_name, int max_players, bool is_private, const String& player_name, int character)
{
    Dictionary msg;
    msg["type"] = "create_lobby";
    msg["name"] = lobby_name;
    msg["maxPlayers"] = max_players;
    msg["isPrivate"] = is_private;
    msg["playerName"] = player_name;
    msg["character"] = character;
    send_message(msg);
}

void NetworkManager::send_message(const Dictionary& msg)
{
    if(socket.is_null() || socket->get_ready_state() != WebSocketPeer::STATE_OPEN) return;
    Variant jsonString = JSON::stringify(msg);
    socket->send_text(jsonString);
}

void NetworkManager::leave_lobby()
{
    Dictionary msg;
    msg["type"] = "leave";
    send_message(msg);
    curr_lobby.clear();
}

void NetworkManager::join_lobby(const String& code, const String& player_name, int character,bool from_list)
{
    Dictionary msg;
    msg["type"] = "join";
    msg["code"] = code;
    msg["playerName"] = player_name;
    msg["character"] = character;
    msg["fromList"] = from_list;
    send_message(msg);
}

void NetworkManager::set_locked(bool locked)
{
    Dictionary msg;
    msg["type"] = "set_locked";
    msg["locked"] = locked;
    send_message(msg);
}

void NetworkManager::kick_player(int id)
{
    Dictionary msg;
    msg["type"] = "kick";
    msg["id"] = id;
    send_message(msg);
}

Dictionary NetworkManager::get_curr_lobb(){return curr_lobby;}

