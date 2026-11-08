#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/engine.hpp>

#include "ErrorPopup.h"

using namespace godot;

void ErrorPopup::_bind_methods(){
    ADD_SIGNAL(MethodInfo("closed"));
};

void ErrorPopup::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    popup = get_node<Control>("%ErrorMessagePopup");
    errorText = get_node<Label>("%ErrorMessage");
    Button* okButt = get_node<Button>("%OkErrorButton");
    if(okButt)okButt->connect("pressed",callable_mp(this,&ErrorPopup::on_Ok_Pressed));

}

void ErrorPopup::createPopup(const String& text)
{
    if(popup && errorText )
    {
        errorText->set_text(text);
        popup->show();
    }
}

void ErrorPopup::on_Ok_Pressed()
{
    hidePopup();
    emit_signal("closed");
}

void ErrorPopup::hidePopup()
{
    if(popup)popup->hide();
}