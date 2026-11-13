#pragma once

#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/input_event.hpp>



namespace godot
{
class MatchMenu: public CanvasLayer
{
GDCLASS(MatchMenu,CanvasLayer);
protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _unhandled_input(const Ref<InputEvent>& event) override;
    void set_is_host(int my_id);
private:
    Button* leave_butt = nullptr;
    Button* stop_game_butt = nullptr;
    void on_leave_butt_pr();
    void on_st_match_butt_pr();
};
}