#include "complete_ring.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>

using namespace godot;

static const float SIDE        = 4.0f;
static const float R_TWO       = 2.6f;
static const float SQUARE_STEP = 0.3f;
static const float FLOOR_H     = 0.6f; 
static const float BASE_H      = 0.3f;  
static const float BASE_MARGIN = 0.4f;
static const float LOGO_FILL   = 0.6f;  
static const float LOGO_PLANE  = 4.0f;  
static const float POST_INSET = 0.35f;  
static const float APRON_OFFSET = 0.01f;

void CompleteRing::_bind_methods(){}

void CompleteRing::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    apron_scene = ResourceLoader::get_singleton()->load("res://Ring/Meshes/ring_apron.tscn");
    if (apron_scene.is_null()) UtilityFunctions::push_error("CompleteRing: δεν φορτώθηκε το ring_apron.tscn");
    post_scene = ResourceLoader::get_singleton()->load("res://Ring/Meshes/pole.tscn");
    if (post_scene.is_null()) UtilityFunctions::push_error("CompleteRing: δεν φορτώθηκε το pole.tscn");
    rope_set_scene = ResourceLoader::get_singleton()->load("res://Ring/Meshes/ropes.tscn");
    if (rope_set_scene.is_null()) UtilityFunctions::push_error("CompleteRing: δεν φορτώθηκε το ropes.tscn");
    aprons = get_node<Node3D>("%Aprons");
    posts = get_node<Node3D>("%Posts");
    rope_sets = get_node<Node3D>("%RopeSets");
    floor = get_node<MeshInstance3D>("%FloorMesh/%Floor");
    base = get_node<MeshInstance3D>("%FloorMesh/%Base");
    logo = get_node<MeshInstance3D>("%FloorMesh/%Logo");
}

float CompleteRing::get_rad() const {return R;}


void CompleteRing::build_ring(int plr_am)
{
    clear_container(aprons);
    clear_container(posts);
    clear_container(rope_sets);
    plr_am = CLAMP(plr_am,2,10);
    N = calc_corn(plr_am);
    R = calc_rad(plr_am);
    build_floor();
    build_aprons();
    build_poles();
    build_ropes();
    is_built = true;
}

void CompleteRing::clear_container(Node3D* container)
{
    for (int i = container->get_child_count() - 1; i >= 0; i--)
    {
        Node* c = container->get_child(i);
        container->remove_child(c);
        c->queue_free();
    }
}

void CompleteRing::make_playable()
{
    if(!is_built) return UtilityFunctions::push_error("Arena Has Not Been Build Yet");
}

Vector3 CompleteRing::get_corner(int i) {return Vector3(R*Math::sin(2*Math::PI*i/N),0,R*Math::cos(2*Math::PI*i/N));}


int CompleteRing::calc_corn(int plr_am)
{
    int tot_corn = plr_am <= 4 ? 4 : plr_am;
    return tot_corn;
}

float CompleteRing::calc_rad(int plr_am)
{
    float tot_rad = N == 4 ? R_TWO+SQUARE_STEP*(plr_am-2.0f) : SIDE/(2.0f*(float)Math::sin(Math::PI/N));
    return tot_rad;
}

Ref<CylinderMesh> CompleteRing::make_prism(float radius, float height)
{
    Ref<CylinderMesh> m;
    m.instantiate();               
    m->set_top_radius(radius);
    m->set_bottom_radius(radius);
    m->set_height(height);
    m->set_radial_segments(N);     
    m->set_rings(0);
    return m;
}

Vector3 CompleteRing::hole_towards(Node3D* pole, Vector3 target)
{
    Vector3 hx = pole->get_transform().xform(Vector3(0.125,0,0));
    Vector3 hz = pole->get_transform().xform(Vector3(0,0,0.125));
    return hx.distance_to(target) < hz.distance_to(target) ? hx : hz;
}

void CompleteRing::build_floor()
{
    floor->set_mesh(make_prism(R, FLOOR_H));
    base->set_mesh(make_prism(R+BASE_MARGIN,BASE_H));
    float inner = R * (float)Math::cos(Math::PI/N);
    float size = 2 * inner * LOGO_FILL;
    float s = size / LOGO_PLANE;
    logo->set_scale(Vector3(s, 1, s));
}


void CompleteRing::build_aprons()
{
    for(int i=0;i<N;i++)
    {
        Vector3 a = get_corner(i);
        Vector3 b = get_corner((i+1)%N);
        Vector3 mid = (a+b)/2;
        float length = a.distance_to(b);
        MeshInstance3D* apron = Object::cast_to<MeshInstance3D>(apron_scene->instantiate());
        if(!apron){UtilityFunctions::push_error("Apron couldnt be loaded"); return;}
        aprons->add_child(apron);   
        apron->set_position(mid + mid.normalized() * APRON_OFFSET);
        apron->set_rotation(Vector3(0, atan2(mid.x, mid.z), 0));
        apron->set_scale(Vector3(length, 1, 1));
        Ref<StandardMaterial3D> mat = apron->get_surface_override_material(0);
        mat = mat->duplicate();
        mat->set_uv1_scale(Vector3(length / 2.4f, 1, 1));
        apron->set_surface_override_material(0, mat);   
    }
}


void CompleteRing::build_poles()
{
    for(int i=0;i<N;i++)
    {
        Vector3 corner = get_corner(i);
        Node3D* pole = Object::cast_to<Node3D>(post_scene->instantiate());
        if(!posts){ UtilityFunctions::push_error("Posts couldnt be loaded");return;}
        posts->add_child(pole);
        Vector3 pos = corner.normalized()*(R-POST_INSET);
        pos.y = FLOOR_H/2;
        pole->set_position(pos);
        float to_center = Math::atan2(-corner.x,-corner.z);
        pole->set_rotation(Vector3(0,to_center-Math::PI/4,0));
    }
}


void CompleteRing::build_ropes()
{
    for(int i=0;i<N;i++)
    {
        Node3D* p1 = Object::cast_to<Node3D>(posts->get_child(i));
        Node3D* p2 = Object::cast_to<Node3D>(posts->get_child((i+1)%N));
        Vector3 a = hole_towards(p1,p2->get_position());
        Vector3 b = hole_towards(p2,p1->get_position());
        Vector3 pos = (a+b)/2;
        Node3D* ropes = Object::cast_to<Node3D>(rope_set_scene->instantiate());
        if(!ropes){UtilityFunctions::push_error("Ropes couldnt be loaded"); return;}
        rope_sets->add_child(ropes);
        float dir_angle = atan2(-(b.z-a.z),(b.x-a.x));
        ropes->set_position(pos);
        ropes->set_scale(Vector3(a.distance_to(b)+0.04,1,1));
        ropes->set_rotation(Vector3(0,dir_angle,0));
    }
}

