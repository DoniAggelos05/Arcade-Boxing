#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <godot_cpp/classes/engine.hpp>
#include "network_manager.h"
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>

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
    ADD_SIGNAL(MethodInfo("match_started"));
    ADD_SIGNAL(MethodInfo("all_connected"));
    ClassDB::bind_method(D_METHOD("all_connected_rpc"), &NetworkManager::all_connected_rpc);
}

void NetworkManager::_ready()
{
    set_process(false);
     if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    Dictionary cfg;
    cfg["rpc_mode"] = MultiplayerAPI::RPC_MODE_AUTHORITY;
    cfg["call_local"] = true;
    cfg["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    cfg["channel"] = 0;
    rpc_config("all_connected_rpc", cfg);
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

        players.clear();
        int my_id = msg.get("id", 0);
        Dictionary me;
        me["name"] = msg.get("playerName", "");
        me["character"] = msg.get("character", 0);
        players[my_id] = me;

        Array peers = msg.get("peers", Array());
        for (int i = 0; i < peers.size(); i++)
        {
            Dictionary p = peers[i];
            int id = p.get("id", 0);
            Dictionary info;
            info["name"] = p.get("name", "");
            info["character"] = p.get("character", 0);
            players[id] = info;
        }
        
        emit_signal("created_lobby",msg);
    }
    else if(type=="error")
    {
        emit_signal("server_error",msg.get("reason",""));
    }
    else if(type=="lobby_closed")
    {
        curr_lobby.clear();
        players.clear();
        emit_signal("lobby_closed");
    }
    else if (type == "peer_joined")
    {
        int id = msg.get("id", 0);
        String name = msg.get("name", "");
        int character = msg.get("character", 0);

        Dictionary info;
        info["name"] = name;
        info["character"] = character;
        players[id] = info;
        emit_signal("peer_joined", id, name, character);
    }
    else if (type == "peer_left")
    {
        int id = msg.get("id", 0);
        players.erase(id);
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
        players.clear();
        emit_signal("kicked");
    }
    else if(type == "match_started")
    {
        roster = msg.get("roster", Array());
        start_webrtc();
        emit_signal("match_started");
    }
    else if(type == "signal")
    {
        int from = msg.get("from",0);
        Dictionary data = msg.get("data",Dictionary());
        Ref<WebRTCPeerConnection> conn = connections.get(from,Variant());
        if(conn.is_null()) return;
        if(data["kind"] == "sdp")
        {
            conn->set_remote_description(data["type"],data["sdp"]);
        }
        else if(data["kind"] == "ice")
        {
            conn->add_ice_candidate(data["media"],data["index"],data["name"]);
        }

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
    players.clear();
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

void NetworkManager::start_match()
{
    Dictionary msg;
    Array roster;
    msg["type"] = "start_match";
    Array ids = players.keys();
    ids.shuffle();
    for(int i=0;i<ids.size();i++)
    {
        int id = ids[i];
        Dictionary info = players[id];
        Dictionary entry;
        entry["id"] = id;
        entry["character"] = info.get("character",0);
        roster.push_back(entry);
    }
    msg["roster"] = roster;
    send_message(msg);
}

Dictionary NetworkManager::get_curr_lobb(){return curr_lobby;}

Dictionary NetworkManager::get_players(){return players;}

Array NetworkManager::get_roster(){return roster;}


void NetworkManager::start_webrtc()
{
    my_id = curr_lobby.get("id", 0);
    rtc.instantiate();
    if(my_id == 1)
    {
        rtc->create_server();
        for(int i=0;i<roster.size();i++)
        {
            Dictionary d = roster[i];
            int id = d.get("id",0);
            if(id != 1) create_conn(id);
        }
    }
    else
    {
        rtc->create_client(my_id);
        Ref<WebRTCPeerConnection> conn = create_conn(1);
        conn->create_offer();
    }
    get_multiplayer()->set_multiplayer_peer(rtc);
    get_multiplayer()->connect("peer_connected",callable_mp(this,&NetworkManager::on_rtc_peer_connected));
}

void NetworkManager::on_rtc_peer_connected(int64_t id)
{
    UtilityFunctions::print("WebRTC connected to ", id);
    if(my_id != 1) return;
    connected_count++;
    if(connected_count == roster.size() - 1) rpc("all_connected_rpc");
}

Ref<WebRTCPeerConnection> NetworkManager::create_conn(int id)
{
    Ref<WebRTCPeerConnection> conn;
    conn.instantiate();
    Dictionary config;
    config["iceServers"] = curr_lobby.get("iceServers",Array());
    conn->initialize(config);
    conn->connect("session_description_created",callable_mp(this,&NetworkManager::on_sdp_created).bind(id));
    conn->connect("ice_candidate_created",callable_mp(this,&NetworkManager::on_ice_created).bind(id));
    rtc->add_peer(conn,id);
    connections[id] = conn;
    return conn;
}

void NetworkManager::on_sdp_created(const String& type, const String& sdp, int id)
{
    Ref<WebRTCPeerConnection> conn = connections.get(id, Variant());
    if(conn.is_null()) return;
    conn->set_local_description(type, sdp);

    Dictionary data;
    data["kind"] = "sdp";
    data["type"] = type;
    data["sdp"] = sdp;

    Dictionary msg;
    msg["type"] = "signal";
    msg["to"] = id;
    msg["data"] = data;
    send_message(msg);
}

void NetworkManager::on_ice_created(const String& media, int index, const String& name, int id)
{
    Dictionary data;
    data["kind"] = "ice";
    data["media"] = media;
    data["index"] = index;
    data["name"] = name;

    Dictionary msg;
    msg["type"] = "signal";
    msg["to"] = id;
    msg["data"] = data;
    send_message(msg);
}

void NetworkManager::all_connected_rpc()
{
    all_connected = true;
    emit_signal("all_connected");
}

bool NetworkManager::is_all_connected(){return all_connected;}
