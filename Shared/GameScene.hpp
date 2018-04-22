#pragma once
#include "BaseScene.hpp"
#include "Model.hpp"
#include "SkyBox.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Map.hpp"
#include "WindowWrapper.hpp"

class PauseMenuChoices {
public:
	static const EnumType RESUME = 0;
	static const EnumType MAIN_MENU = 1;
	static const EnumType EXIT_GAME = 2;

	static const EnumType FIRST = RESUME;
	static const EnumType LAST = EXIT_GAME;
};


class GameReturnCodes : public ReturnCodes {
public:
    static const EnumType MAIN_MENU = 4;
    static const EnumType PLAYER_DEAD = 5;
};


class GameScene : public BaseScene {
private:
	// Const variables must be at the top of a class definition so that they can be used in the initialiser list in the constructor
	// Const variables
	const float player_square_dimension = 2.0f;
	const float player_height = 5.0f;

	// Map boundaries for node placement
	const int x_lower_bound = -90;
	const int x_upper_including = 94;
	const int z_lower_bound = -90;
	const int z_upper_including = 92;

	// Map boundaries for collision detection
	const float x_lower_collision = -87.0f;
	const float x_upper_collision = 91.8f;
	const float z_lower_collision = -88.5f;
	const float z_upper_collision = 90.5f;

public:
	// Constructors and Destructors
	GameScene(WindowWrapper* window, Camera* camera);
	virtual ~GameScene();

	// Deleted Versions
	GameScene(const GameScene& other) = delete;
	void operator=(const GameScene& other) = delete;

	// Instance Management
    static GameScene* get_instance(WindowWrapper* window = nullptr, Camera* camera = nullptr){
		if (!GameScene::instance) { GameScene::instance = new GameScene((!window ? &WindowWrapper::get_instance() : window), camera); }
		return GameScene::instance;
    }
    
    static void delete_instance() {
        delete GameScene::instance;
        GameScene::instance = nullptr;
    }

public:
	// Virtual Functions
	virtual void bind_callbacks();

    virtual int main_loop();
    virtual void pre_render();
    virtual void render();
	virtual inline void post_render() { glfwSwapBuffers(window->get_window()); }
    
    inline void set_score_getter(Attribute<UInt>* new_getter) { player_score_getter = new_getter; }

private:
    static GameScene* instance;

	// Private Member Variables
	UInt framebuffer;
	UInt cube_map;

	Player player;
	SkyBox sky;
	Model terrain;
	Cube sun;

	std::vector<Enemy*> enemies;
	Map node_map;
    
    Attribute<UInt>* player_score_getter;

	bool pause_activated = false;
	bool use_shadows = true;

	Text wave_text;
	Text pause_menu_title;
	Text resume_text;
	Text exit_to_main_menu;
	Text exit_game;

	EnumType selected_option = PauseMenuChoices::FIRST;
	float wave_text_timer = 0.0f;
	size_t wave_number = 1;

	// Private Member Functions
    void init();
	void handle_held_keys();

	void render_framebuffer();
	void setup_framebuffer();
    
	void spawn_wave();
    void spawn_enemy();
    void set_enemy_pathfind();
    void evaluate_enemy_collisions();
	void evaluate_player_collisions();
    void update_enemies();
    
    std::vector<std::vector<Node*>> get_nodes();
    void pathfind_test(const std::vector<Node*>& path);
    void select_node();

	void display_wave_text();

	void change_selection(bool up);
	void select_menu_item();

	inline bool using_shadows() {
		return (dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()))->get_light_count() > 0) && use_shadows;
	}

	inline bool is_paused() { return pause_activated; }
	
public:
    // Callback functions
    void mouse_movement(GLFWwindow* window, double x_pos, double y_pos);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	inline void window_close(GLFWwindow* window) { return_code = ReturnCodes::EXIT_GAME; }
    void mouse_button(GLFWwindow* window, int button, int action, int mods);
    
    static inline void MOUSE_MOVEMENT_HELPER(GLFWwindow* window, double x_pos, double y_pos){
        GameScene::get_instance()->mouse_movement(window, x_pos, y_pos);
    }

	static inline void KEY_CALLBACK_HELPER(GLFWwindow* window, int key, int scancode, int action, int mode) {
		GameScene::get_instance()->key_callback(window, key, scancode, action, mode);
	}

	static inline void WINDOW_CLOSE_HELPER(GLFWwindow* window) {
		GameScene::get_instance()->window_close(window);
	}
    
    static inline void MOUSE_KEY_HELPER(GLFWwindow* window, int button, int action, int mods){
        GameScene::get_instance()->mouse_button(window, button, action, mods);
    }
};
