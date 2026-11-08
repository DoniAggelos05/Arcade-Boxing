#pragma once

#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/texture_rect.hpp>

namespace godot
{

class LoadingScreen: public CanvasLayer{
    GDCLASS(LoadingScreen,CanvasLayer);

protected:
    static void _bind_methods(); 
public:
    void _ready() override;
    void _process(double delta) override;
    void show_loading(const String& text);   
    void hide_loading();   

private:
    Control* loadScreen = nullptr;
    Label* loadText = nullptr;
    Label* movingDots = nullptr;
    TextureRect* spinner = nullptr;
    double dots_timer; 
    int dot_count;
};

}