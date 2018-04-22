#pragma once
#include "BaseScene.hpp"
#include "Cuboid.hpp"
#include "Text.hpp"
#include "WindowWrapper.hpp"
#include "AttributeParser.hpp"


enum OptionsMenuChoices {
	SHADOWS,
	MULTISAMPLING,
	FULLSCREEN,
	RETURN_TO_MAIN_MENU,

	FIRST = SHADOWS,
	LAST = RETURN_TO_MAIN_MENU
};


class OptionsReturnCodes : public ReturnCodes {
public:
    static const EnumType MAIN_MENU = 6;
};


class OptionsScene : public BaseScene {
private:
	// Const variables
	const float column_width = 1.0f;
	const float column_length = 31.0f;
	const float background_dimension = 65.0f;
	const float fullscreen_text_height = -6.0f;
	const float multisample_text_height = -2.0f;
	const float shadow_text_height = 2.0f;

public:
    // Constructors and Destructors
	OptionsScene(WindowWrapper* window, Camera* camera);
    virtual ~OptionsScene() {}

    // Deleted Versions
	OptionsScene(const OptionsScene& other) = delete;
    void operator=(const OptionsScene& other) = delete;
    
    // Instance Management
    
    static OptionsScene* get_instance(WindowWrapper* window = nullptr, Camera* camera = nullptr){
		if (!OptionsScene::instance) { OptionsScene::instance = new OptionsScene((!window ? &WindowWrapper::get_instance() : window), camera); }
		return OptionsScene::instance;
    }
    
    static void delete_instance() {
        delete OptionsScene::instance;
		OptionsScene::instance = nullptr;
    }
    
public:
    // Virtual Functions
    virtual void bind_callbacks();
    
    virtual int main_loop();
    virtual void pre_render();
    virtual void render();
    virtual inline void post_render() { glfwSwapBuffers(window->get_window()); }
    
private:
    static OptionsScene* instance;

    // Private Member Variables
	AttributeParser parser;
    UInt framebuffer;
    UInt cube_map;
	EnumType choice = OptionsMenuChoices::LAST;
    bool use_shadows = true;
	int initial_multisampling;
    
    Cuboid left_column;
    Cuboid right_column;
    Cuboid top_column;
    Cuboid bottom_column;
    Cuboid background;
    
    Text title;
	Text return_to_main_menu;

	Text fullscreen;
	Text fullscreen_status;

	Text multisampling;
	Text multisample_value;

	Text shadows;
	Text shadow_status;

	Text effect;
	bool render_effect = false;

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

	void update_fullscreen_status();
	void update_multisample_value();
	void update_shadows_value();

public:
    // Callback functions
    void mouse_movement(GLFWwindow* window, double x_pos, double y_pos);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    inline void window_close(GLFWwindow* window) { return_code = ReturnCodes::EXIT_GAME; }
    
    static inline void MOUSE_MOVEMENT_HELPER(GLFWwindow* window, double x_pos, double y_pos){
		OptionsScene::get_instance()->mouse_movement(window, x_pos, y_pos);
    }

	static inline void KEY_CALLBACK_HELPER(GLFWwindow* window, int key, int scancode, int action, int mode) {
		OptionsScene::get_instance()->key_callback(window, key, scancode, action, mode);
	}

	static inline void WINDOW_CLOSE_HELPER(GLFWwindow* window) {
		OptionsScene::get_instance()->window_close(window);
	}
};
