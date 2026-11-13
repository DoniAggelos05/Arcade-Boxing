#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/viewport.hpp>

#include "match_menu.h"


using namespace godot;

void MatchMenu::_bind_methods(){
    ADD_SIGNAL(MethodInfo("leave_pressed"));
    ADD_SIGNAL(MethodInfo("stop_game_pressed"));
}

void MatchMenu::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    set_visible(false);
    leave_butt = get_node<Button>("%LeaveButton");
    stop_game_butt = get_node<Button>("%StopGameButton");
    if(!stop_game_butt || !leave_butt) return;
    leave_butt->connect("pressed",callable_mp(this,&MatchMenu::on_leave_butt_pr));
    stop_game_butt->connect("pressed",callable_mp(this,&MatchMenu::on_st_match_butt_pr));


}

void MatchMenu::set_is_host(int id)
{
    if(stop_game_butt) stop_game_butt->set_visible(id == 1);
}

void MatchMenu::_unhandled_input(const Ref<InputEvent>& event)
{
    if(event->is_action_pressed("ui_cancel"))
    {
        set_visible(!is_visible());   
        get_viewport()->set_input_as_handled();
    }
}

void MatchMenu::on_leave_butt_pr()
{
    set_visible(false);
    emit_signal("leave_pressed");
}

void MatchMenu::on_st_match_butt_pr()
{
    set_visible(false);
    emit_signal("stop_game_pressed");
}