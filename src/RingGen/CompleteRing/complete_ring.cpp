#include "complete_ring.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/convex_polygon_shape3d.hpp>
#include<godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/box_shape3d.hpp>

using namespace godot;

static const float SIDE        = 4.0f;
static const float R_TWO       = 2.6f;
static const float SQUARE_STEP = 0.3f;
static const float FLOOR_H     = 0.6f; 
static const float BASE_H      = 0.3f;  
static const float BASE_MARGIN = 0.4f;
static const float LOGO_FILL   = 0.6f;  
static const float LOGO_PLANE  = 4.0f;  
static const float RING_SCALE = 1.3f; 
static const float POST_INSET = 0.35f;  
static const float APRON_OFFSET = 0.01f;
static const float WALL_OFFSET = 0.15f;
static const float WALL_HEIGHT = 0.9f;
static const float WALL_THICK = 0.2f;
static const float SPAWN_INSET = 0.5f;

static const Color CORNER_COLORS[10] = {
    Color(0.85, 0.15, 0.15),Color(0.15, 0.35, 0.85),Color(0.15, 0.7, 0.25),
    Color(0.9, 0.75, 0.1),Color(0.6, 0.2, 0.75),Color(0.95, 0.5, 0.1),
    Color(0.1, 0.75, 0.75), Color(0.9, 0.35, 0.6),Color(0.5, 0.5, 0.5),Color(0.95, 0.95, 0.95)
};

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
    physics = get_node<Node3D>("%Physics");
}

float CompleteRing::get_rad() const {return R;}


void CompleteRing::build_ring(int plr_am)
{
    clear_container(aprons);
    clear_container(posts);
    clear_container(rope_sets);
    plr_am = CLAMP(plr_am,2,10);
    P=plr_am;
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

void CompleteRing::make_playable(const Array& players)
{
    if(!is_built) return UtilityFunctions::push_error("Arena den exei xtustei akoma");
    if(players.size() != P) return UtilityFunctions::push_error("Lathos timh players");
    build_floor_coll();
    build_invis_walls();
    for(int i=0;i<P;i++)
    {
            set_corn_color(i,CORNER_COLORS[i]);
            Node3D* player = Object::cast_to<Node3D>(players[i]);
            if(!player){UtilityFunctions::push_error("Den einai o paixths Node3d"); continue;}
            player->set_global_transform(get_global_transform()*calc_spawn_pos(i));
    }
}

void CompleteRing::build_floor_coll()
{
    StaticBody3D* floor_bod = memnew(StaticBody3D);
    CollisionShape3D* floor_coll = memnew(CollisionShape3D);
    floor_coll->set_shape(floor->get_mesh()->create_convex_shape());
    floor_bod->add_child(floor_coll);
    physics->add_child(floor_bod);
}

void CompleteRing::build_invis_walls()
{
    for(int i=0;i<N;i++)
    {
        Ref<BoxShape3D> box;
        box.instantiate();
        Vector3 a = get_corner(i);
        Vector3 b = get_corner((i+1)%N);
        Vector3 mid = (a+b)/2;
        float length = a.distance_to(b);
        StaticBody3D* wall_bod = memnew(StaticBody3D);
        CollisionShape3D* wall_coll = memnew(CollisionShape3D);
        box->set_size(Vector3(length, WALL_HEIGHT, WALL_THICK));
        Vector3 pos = mid.normalized() * (mid.length()+WALL_OFFSET-WALL_THICK/2);
        pos.y = FLOOR_H/2 + WALL_HEIGHT/2 ; 
        wall_coll->set_shape(box);
        wall_bod->add_child(wall_coll);
        wall_bod->set_position(pos);
        wall_bod->set_rotation(Vector3(0, atan2(mid.x, mid.z), 0));
        physics->add_child(wall_bod);
    }
}

int CompleteRing::corner_of(int i){return (int)Math::round((float)i*N/P)%N;}

Transform3D CompleteRing::calc_spawn_pos(int i)
{
    if(i<0 || i>=P){UtilityFunctions::push_error("Den tha eprepe na uparxei paikths me tetoio id"); return Transform3D();} 
    Vector3 pl_cor = get_corner(corner_of(i));
    Vector3 pl_pos = pl_cor.normalized() * (R-(POST_INSET+SPAWN_INSET));
    pl_pos.y = FLOOR_H/2;
    Transform3D pl_tr;
    pl_tr.origin = pl_pos;   
    return pl_tr.looking_at(Vector3(0,FLOOR_H/2,0));
}

void CompleteRing::set_corn_color(int i ,Color color)
{
    Node3D* pole = Object::cast_to<Node3D>(posts->get_child(corner_of(i)));
    MeshInstance3D* pad = pole->get_node<MeshInstance3D>("%TopPad");
    if(!pad) return;
    Ref<StandardMaterial3D> mat = pad->get_surface_override_material(0);
    if(mat.is_null()) return;
    mat = mat->duplicate();
    mat->set_albedo(color);
    pad->set_surface_override_material(0,mat);
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
    return tot_rad*RING_SCALE;
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
        if(!pole){ UtilityFunctions::push_error("Posts couldnt be loaded");return;}
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

