#pragma once
#include "GameHelpers.hpp"
#include "Camera.hpp"
#include "WindowWrapper.hpp"
#include "GameScene.hpp"
#include "MenuScene.hpp"

class Application {
public:
	Application();
	Application(const Application& other) = delete;
	void operator=(const Application& other) = delete;

	// This not being here cause so many memory leaks
	// RIP 30/07/2017
	~Application();

	void start();

	EnumType main_menu();
    std::pair<EnumType, UInt> game_scene();
	EnumType options_scene();
    EnumType death_scene(const UInt& score);

	void run_main_menu();
    void run_game_scene();
	void run_options_scene();
    void run_death_scene(const UInt& score);
    
private:
	WindowWrapper* window;
	Camera* camera;
};
