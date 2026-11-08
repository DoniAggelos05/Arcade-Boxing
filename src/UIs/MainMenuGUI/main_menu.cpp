#include "main_menu.h"
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


using namespace godot;

void MainMenu::_bind_methods(){};

void MainMenu::_ready(){
    //if game hasnt started and in editor done play
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    //give nodes to pointers when game is in ready state
    menu_content = get_node<Control>("%MenuContent"); 
    settings_menu = get_node<Control>("%SettingsMenu");
    error_sc = get_node<ErrorPopup>("/root/ErrorPopup");
    load_sc = get_node<LoadingScreen>("/root/LoadingScreen");
    netMan = get_node<NetworkManager>("/root/NetworkManagerGlobal");
    Button* play_on_butt = get_node<Button>("%PlayOnlineButton");
    Button* play_off_butt = get_node<Button>("%PlayOfflineButton");
    Button* opt_butt = get_node<Button>("%OptionsButton");
    Button* quit_butt = get_node<Button>("%QuitButton");
    //when pressed signal is heard
    play_on_butt->connect("pressed", callable_mp(this, &MainMenu::on_playonline_pressed));
    play_off_butt->connect("pressed", callable_mp(this, &MainMenu::on_playoffline_pressed));
    opt_butt->connect("pressed", callable_mp(this, &MainMenu::on_options_pressed));
    quit_butt->connect("pressed",callable_mp(this,&MainMenu::on_quit_pressed));
    if(settings_menu)settings_menu->connect("closed", callable_mp(this, &MainMenu::on_settings_close));
    if(netMan)netMan->connect("connected",callable_mp(this,&MainMenu::on_connected));
    if(netMan)netMan->connect("connection_closed",callable_mp(this,&MainMenu::on_connection_closed));
}

//button actions in main menu
void MainMenu::on_playoffline_pressed()
{
    UtilityFunctions::print("Play Offline pressed");
}

void MainMenu::on_playonline_pressed()
{
    if(load_sc)load_sc->show_loading("Connecting");
    if(netMan)netMan->connect_to_serv();
}

void MainMenu::on_options_pressed()
{
    if (menu_content) menu_content->hide();
    if(settings_menu) settings_menu->show();
}

void MainMenu::on_quit_pressed()
{
    get_tree()->quit();
}

void MainMenu::on_settings_close()
{
    if(menu_content) menu_content->show();
}

void MainMenu::on_connected()
{
    if(load_sc)load_sc->show_loading("Loading  Lobbies");
    get_tree()->change_scene_to_file("res://UserInterfaces/Lobbies/lobby_list.tscn");
}

void MainMenu::on_connection_closed()
{
    if(load_sc)load_sc->hide_loading();
    if(error_sc)error_sc->createPopup("ConnectionFailedBruv");
}



