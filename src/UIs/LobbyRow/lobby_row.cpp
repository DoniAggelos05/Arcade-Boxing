#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/engine.hpp>

#include "Lobby_row.h"


using namespace godot;

void LobbyRow::_bind_methods(){
    ADD_SIGNAL(MethodInfo("join_requested",
        PropertyInfo(Variant::STRING, "code"),
        PropertyInfo(Variant::BOOL,   "from_list")));
};

void LobbyRow::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    name_text = get_node<Label>("%NameText");
    players_amount = get_node<Label>("%PlayersText");
    join_butt = get_node<Button>("%JoinButton");
    if(join_butt) join_butt->connect("pressed",callable_mp(this,&LobbyRow::_on_join_butt_press));
    
}

void LobbyRow::add_data(const Dictionary& data)
{
    if (!name_text || !players_amount || !join_butt) return;
    code = data.get("code","");
    name_text->set_text(data.get("name",""));
    int players = data.get("players", 0);
    int max_players = data.get("maxPlayers", 0);
    players_amount->set_text(String::num_int64(players)+"/"+String::num_int64(max_players));
    join_butt->set_disabled(players >= max_players);
}

void LobbyRow::_on_join_butt_press()
{
    emit_signal("join_requested", code, true);
}
