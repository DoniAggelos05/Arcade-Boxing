#pragma once 
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <godot_cpp/classes/web_rtc_multiplayer_peer.hpp>
#include <godot_cpp/classes/web_rtc_peer_connection.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>


namespace godot{

class NetworkManager: public Node{
GDCLASS(NetworkManager,Node);

protected:
    static void _bind_methods();
public:
    void _ready() override;
    void _process(double delta) override;
    void connect_to_serv();
    void disc_from_serv();
    void request_lobbies();
    void create_lobby(const String& lobby_name, int max_players, bool is_private,const String& player_name, int character);
    void leave_lobby();
    void join_lobby(const String& code, const String& player_name, int character,bool from_list);
    void set_locked(bool lock);
    void kick_player(int id);
    void start_match();
    Dictionary get_curr_lobb();
    Dictionary get_players();
    Array get_roster();
    void all_connected_rpc();
    bool is_all_connected();
    void close_webrtc();
    void end_match();
    int get_id();

private:
//socket gia thn sundesh me to signaling 
    Ref<WebSocketPeer> socket;
    Ref<WebRTCMultiplayerPeer> rtc;
    bool is_connecting = false;  
    bool connected = false;
    bool all_connected = false;
    double connect_timer = 0.0;
    int my_id = 0;
    int connected_count = 0;
    Dictionary curr_lobby;
    Dictionary players;
    Dictionary connections;
    Array roster;
    void handle_message(const Dictionary& msg);
    void send_message(const Dictionary& msg);
    void start_webrtc();
    Ref<WebRTCPeerConnection> create_conn(int id);
    void on_sdp_created(const String& type, const String& sdp, int id);
    void on_ice_created(const String& media, int index, const String& name, int id);
    void on_rtc_peer_connected(int64_t id);
};


}