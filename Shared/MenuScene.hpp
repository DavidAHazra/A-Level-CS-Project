#pragma once
#include "BaseScene.hpp"
#include "Cuboid.hpp"
#include "Text.hpp"
#include "WindowWrapper.hpp"


class MenuChoice {
public:
	static const EnumType PLAY = 0;
	static const EnumType OPTIONS = 1;
	static const EnumType EXIT = 2;

	static const EnumType FIRST = PLAY;
	static const EnumType LAST = EXIT;
};


class MainMenuReturnCodes : public ReturnCodes {
public:
    static const EnumType PLAY = 2;
    static const EnumType OPTIONS = 3;
};


class MenuScene : public BaseScene {
private:
	// Const variables
	const float column_width = 1.0f;
	const float column_length = 31.0f;
	const float background_dimension = 65.0f;

public:
    // Constructors and Destructors
    MenuScene(WindowWrapper* window, Camera* camera);
    virtual ~MenuScene() {}

    // Deleted Versions
    MenuScene(const MenuScene& other) = delete;
    void operator=(const MenuScene& other) = delete;
    
    // Instance Management
    
    static MenuScene* get_instance(WindowWrapper* window = nullptr, Camera* camera = nullptr){
		if (!MenuScene::instance) { MenuScene::instance = new MenuScene((!window ? &WindowWrapper::get_instance() : window), camera); }
		return MenuScene::instance;
    }
    
    static void delete_instance() {
        delete MenuScene::instance;
        MenuScene::instance = nullptr;
    }
    
public:
    // Virtual Functions
    virtual void bind_callbacks();
    
    virtual int main_loop();
    virtual void pre_render();
    virtual void render();
    virtual inline void post_render() { glfwSwapBuffers(window->get_window()); }
    
private:
    static MenuScene* instance;

    // Private Member Variables
    UInt framebuffer;
    UInt cube_map;
	EnumType choice = MenuChoice::FIRST;
    bool use_shadows = true;
    
    Cuboid left_column;
    Cuboid right_column;
    Cuboid top_column;
    Cuboid bottom_column;
    Cuboid background;
    
    Text title;
    Text play;
    Text options;
    Text exit;

    // Private Member Functions
    void init();
	void handle_held_keys();

    inline bool using_shadows() {
        return (dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()))->get_light_count() > 0) && use_shadows;
    }

    void render_framebuffer();
	void setup_framebuffer();

	void change_choice(bool increment);
	void select_choice();

public:
    // Callback functions
    void mouse_movement(GLFWwindow* window, double x_pos, double y_pos);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    inline void window_close(GLFWwindow* window) { return_code = ReturnCodes::EXIT_GAME; }
    
    static inline void MOUSE_MOVEMENT_HELPER(GLFWwindow* window, double x_pos, double y_pos){
		MenuScene::get_instance()->mouse_movement(window, x_pos, y_pos);
    }

	static inline void KEY_CALLBACK_HELPER(GLFWwindow* window, int key, int scancode, int action, int mode) {
		MenuScene::get_instance()->key_callback(window, key, scancode, action, mode);
	}

	static inline void WINDOW_CLOSE_HELPER(GLFWwindow* window) {
		MenuScene::get_instance()->window_close(window);
	}
};
