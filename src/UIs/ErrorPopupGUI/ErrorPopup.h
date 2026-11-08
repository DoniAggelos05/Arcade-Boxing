#pragma once
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>




namespace godot{

class ErrorPopup:public CanvasLayer{

GDCLASS(ErrorPopup,CanvasLayer);

protected:
    static void _bind_methods();
 
public:
    void _ready() override;    
    void createPopup(const String& text);
    void hidePopup();

private:
    Control* popup = nullptr;
    Label* errorText = nullptr;
    void on_Ok_Pressed();

};


}