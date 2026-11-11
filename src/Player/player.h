#pragma once

#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/node3d.hpp>


namespace godot{
class Player: public CharacterBody3D
{
GDCLASS(Player,CharacterBody3D);
protected:
    static void _bind_methods();
public:
    void _ready() override;
    void set_plr_model(int c);
private:
    Node3D* model_con = nullptr;
};

}