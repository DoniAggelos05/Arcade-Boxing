#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/world_environment.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/environment.hpp>
#include "RingGen/CompleteRing/complete_ring.h"



namespace godot
{

class MenuBackground: public Node3D
{
GDCLASS(MenuBackground,Node3D);

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _process(double delta) override;
    void set_active(bool on);

private:
    int last_pl=0;
    float exposure_on = 1.0f;
    WorldEnvironment* background = nullptr;
    Ref<Environment> env;
    Vector3 base_cam_pos;
    CompleteRing* ring = nullptr;
    Node3D* camera_rig = nullptr;
    Camera3D* camera = nullptr;
    Timer* timer = nullptr;
    void fade_in();
    void on_timer_reset();
    void place_camera();
    void new_ring();

};

}