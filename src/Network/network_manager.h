#pragma once 
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>


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
    Dictionary get_curr_lobb();
    Dictionary get_players();
private:
//socket gia thn sundesh me to signaling 
    Ref<WebSocketPeer> socket;
    bool is_connecting = false;  
    bool connected = false;
    double connect_timer = 0.0;
    Dictionary curr_lobby;
    Dictionary players;
    void handle_message(const Dictionary& msg);
    void send_message(const Dictionary& msg);
};


}