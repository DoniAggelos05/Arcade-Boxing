#pragma once
#include <godot_cpp/classes/control.hpp>
#include "UIs/LoadingScreenGUI/loading_screen.h"
#include "UIs/ErrorPopupGUI/ErrorPopup.h"
#include "Network/network_manager.h"



namespace godot{

class MainMenu: public Control{
    GDCLASS(MainMenu,Control);


protected:
	static void _bind_methods();

public:
    void _ready() override;


private:
    //initiate wharever we will need later... main menu and settings menu to show and hide
    Control* menu_content = nullptr;
    Control* settings_menu = nullptr;
    LoadingScreen* load_sc = nullptr;
    ErrorPopup* error_sc = nullptr;
    NetworkManager* netMan = nullptr;
    
    void on_playonline_pressed();
    void on_playoffline_pressed();
    void on_options_pressed();
    void on_quit_pressed();
    void on_settings_close();
    void on_connected();
    void on_connection_closed();

};    
}