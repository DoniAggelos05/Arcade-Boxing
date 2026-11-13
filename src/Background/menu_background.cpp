#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/tween.hpp>
#include "menu_background.h"

using namespace godot;

static const float ROT_SPEED = 0.15f;
static const float REF_R = 4.0f;
static const float FADE_TIME = 1.5f;

void MenuBackground::_bind_methods(){}

void MenuBackground::_ready()
{
    set_process(false);
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }    
    UtilityFunctions::randomize();
    background = get_node<WorldEnvironment>("%Background");
    ring = get_node<CompleteRing>("%CompleteRing");
    camera_rig = get_node<Node3D>("%CameraRig");
    camera = get_node<Camera3D>("%Camera3D");
    timer = get_node<Timer>("%CycleTimer");
    env = background->get_environment();
    exposure_on = env->get_tonemap_exposure();
    env->set_tonemap_exposure(0.0);
    base_cam_pos = camera->get_position();
    timer->connect("timeout",callable_mp(this,&MenuBackground::on_timer_reset));
    new_ring();
    fade_in();
    set_process(true);

}

void MenuBackground::_process(double delta)
{
    camera_rig->rotate_y(ROT_SPEED * delta);
}

void MenuBackground::on_timer_reset()
{
    Ref<Tween> t = create_tween();
    t->tween_property(env.ptr(),"tonemap_exposure",0.0f,FADE_TIME);
    t->tween_callback(callable_mp(this,&MenuBackground::new_ring));
    t->tween_callback(callable_mp(this,&MenuBackground::fade_in));
}

void MenuBackground::fade_in()
{
    Ref<Tween> t = create_tween();
    t->tween_property(env.ptr(),"tonemap_exposure",exposure_on,FADE_TIME);
    t->tween_callback(callable_mp(timer, &Timer::start).bind(-1.0));
}

void MenuBackground::new_ring()
{
    int players;
    do
    {
        players = (int)UtilityFunctions::randi_range(4, 10);
    } while(last_pl==players);
    ring->build_ring(players);
    last_pl = players;
    place_camera();
}

void MenuBackground::place_camera()
{
    float r = ring->get_rad();
    float k = MAX(1.0f, r/REF_R);
    camera->set_position(base_cam_pos * k);
}

void MenuBackground::set_active(bool on)
{
    set_visible(on);
    set_process_mode(on ? PROCESS_MODE_INHERIT : PROCESS_MODE_DISABLED);
    if(camera)
    {
        if(on) camera->make_current();
        else camera->clear_current(false);
    }
    if(background) background->set_environment(on ? env : Ref<Environment>());
}