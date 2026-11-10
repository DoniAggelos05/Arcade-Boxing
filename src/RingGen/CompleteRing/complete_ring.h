#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/cylinder_mesh.hpp>


namespace godot
{

class CompleteRing: public Node3D
{
GDCLASS(CompleteRing,Node3D);

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void build_ring(int plr_am);
    void make_playable(const Array& players);
    void clear_container(Node3D* container);
    float get_rad() const;

private:
    Ref<PackedScene> apron_scene;
    Ref<PackedScene> post_scene;
    Ref<PackedScene> rope_set_scene;
    Node3D* aprons = nullptr;
    Node3D* posts = nullptr;
    Node3D* rope_sets = nullptr;
    Node3D* physics = nullptr;
    MeshInstance3D* floor = nullptr;
    MeshInstance3D* base = nullptr;
    MeshInstance3D* logo = nullptr;
    int N = 0;
    int P = 0;
    float R = 0.0f;
    bool is_built = false;
    int calc_corn(int plr_am);
    float calc_rad(int plr_am);
    Ref<CylinderMesh> make_prism(float radius, float height);
    void build_floor();
    Vector3 get_corner(int i);
    void build_aprons();
    void build_poles();
    Vector3 hole_towards(Node3D* pole, Vector3 target); 
    void build_ropes();
    void build_floor_coll();
    void build_invis_walls();
    int corner_of(int i);
    Transform3D calc_spawn_pos(int i);
    void set_corn_color(int i,Color color);
};
}