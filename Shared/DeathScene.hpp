#pragma once
#include "BaseScene.hpp"
#include "Cuboid.hpp"
#include "Text.hpp"
#include "WindowWrapper.hpp"
#include "AttributeParser.hpp"


enum Selection {
    USERNAME,
    DONE
};


class DeathSceneReturnCodes : public ReturnCodes {
public:
    static const EnumType MAIN_MENU = 7;
};


class DeathScene : public BaseScene {
private:
    const float column_width = 1.0f;
    const float column_length = 50.0f;
    const float background_dimension = 100.0f;
    const UShort max_char_count = 6;

public:
    // Constructors and Destructors
	DeathScene(WindowWrapper* window, Camera* camera);
    virtual ~DeathScene() {}

    // Deleted Versions
	DeathScene(const DeathScene& other) = delete;
    void operator=(const DeathScene& other) = delete;
    
    // Instance Management
    
    static DeathScene* get_instance(WindowWrapper* window = nullptr, Camera* camera = nullptr){
		if (!DeathScene::instance) { DeathScene::instance = new DeathScene((!window ? &WindowWrapper::get_instance() : window), camera); }
		return DeathScene::instance;
    }
    
    static void delete_instance() {
        delete DeathScene::instance;
		DeathScene::instance = nullptr;
    }
    
public:
    // Virtual Functions
    virtual void bind_callbacks();
    
    virtual int main_loop();
    virtual void pre_render();
    virtual void render();
    virtual inline void post_render() { glfwSwapBuffers(window->get_window()); }
    
    void set_score(const UInt& new_score);
    
private:
    static DeathScene* instance;

    // Private Member Variables
	AttributeParser parser;
    UInt framebuffer;
    UInt cube_map;
    bool use_shadows = true;
    
    Selection selection = Selection::USERNAME;
    
    UInt score = 0;
    bool typing = true;
    
    Cuboid left_column;
    Cuboid right_column;
    Cuboid top_column;
    Cuboid bottom_column;
    Cuboid background;

    Text you_are_dead;
    Text score_text;
    Text please_type;
    Text username_text;
    Text done_text;
    
    Text showing_top_5;
    
    Text first_name;
    Text second_name;
    Text third_name;
    Text fourth_name;
    Text fifth_name;
    
    Text first_score;
    Text second_score;
    Text third_score;
    Text fourth_score;
    Text fifth_score;
    
    Text return_to_main_menu;
        
    // Private Member Functions
    void init();
	void handle_held_keys();

    inline bool using_shadows() {
        return (dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()))->get_light_count() > 0) && use_shadows;
    }

    void render_framebuffer();
	void setup_framebuffer();
    
    void update_username(const char new_char);
    
    void load_stats();

public:
    // Callback functions
    void mouse_movement(GLFWwindow* window, double x_pos, double y_pos);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    inline void window_close(GLFWwindow* window) { return_code = ReturnCodes::EXIT_GAME; }
    
    static inline void MOUSE_MOVEMENT_HELPER(GLFWwindow* window, double x_pos, double y_pos){
		DeathScene::get_instance()->mouse_movement(window, x_pos, y_pos);
    }

	static inline void KEY_CALLBACK_HELPER(GLFWwindow* window, int key, int scancode, int action, int mode) {
		DeathScene::get_instance()->key_callback(window, key, scancode, action, mode);
	}

	static inline void WINDOW_CLOSE_HELPER(GLFWwindow* window) {
		DeathScene::get_instance()->window_close(window);
	}
};
