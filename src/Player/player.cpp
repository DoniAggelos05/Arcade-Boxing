#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "Characters/characters.h"
#include "player.h"


using namespace godot;

void Player::_bind_methods(){}


void Player::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    model_con = get_node<Node3D>("%Model");
}

void Player::set_plr_model(int c)
{

    if(!model_con) return;
    if(c < 0 || c > 3) c = 0;
    
    for(int i = model_con->get_child_count() - 1; i >= 0; i--)
    {
        Node* old = model_con->get_child(i);
        model_con->remove_child(old);
        old->queue_free();
    }

    Ref<PackedScene> model_scene = ResourceLoader::get_singleton()->load(CHARACTER_SCENES[c]);
    if (model_scene.is_null()) return UtilityFunctions::push_error("Model_scene: den forthothike modelo");
    Node3D* model = Object::cast_to<Node3D>(model_scene->instantiate());
    if(!model) return UtilityFunctions::push_error("Player: to modelo den einai Node3D");

    model_con->add_child(model);
    
}
