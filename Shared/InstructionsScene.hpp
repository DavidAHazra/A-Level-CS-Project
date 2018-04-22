#pragma once
#include "BaseScene.hpp"
#include "Cuboid.hpp"
#include "Text.hpp"
#include "WindowWrapper.hpp"


class InstructionsScene : public BaseScene {
    const int timeout = 10;

#ifdef IS_APPLE
	const float title_size = 1.0f;
	const float standard_size = 0.4f;
#else
	const float title_size = 0.5f;
	const float standard_size = 0.2f;
#endif


public:
    // Constructors and Destructors
	InstructionsScene(WindowWrapper* window, Camera* camera);
    virtual ~InstructionsScene() {}

    // Deleted Versions
	InstructionsScene(const InstructionsScene& other) = delete;
    void operator=(const InstructionsScene& other) = delete;
    
    // Instance Management
    static InstructionsScene& get_instance(WindowWrapper* window = nullptr, Camera* camera = nullptr){
		static InstructionsScene instance((!window ? &WindowWrapper::get_instance() : window), camera);
		return *&instance;	// TODO: *& ? xD
    }
        
public:
    // Virtual Functions
    virtual void bind_callbacks();
    
    virtual int main_loop();
    virtual void pre_render();
    virtual void render();
    virtual inline void post_render() { glfwSwapBuffers(window->get_window()); }
    
private:
    // Private Member Variables        
    Text game_name;
    Text movement_text;
    Text arrow_key;
    Text change_projectile_text;
    Text instructions;
    Text exit_text;
    
    // Private Member Functions
    void init();

public:
    // Callback functions
    inline void window_close(GLFWwindow* window) { return_code = ReturnCodes::EXIT_GAME; }
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

    static inline void WINDOW_CLOSE_HELPER(GLFWwindow* window) {
		InstructionsScene::get_instance().window_close(window);
	}
    
    static inline void KEY_CALLBACK_HELPER(GLFWwindow* window, int key, int scancode, int action, int mode) {
        InstructionsScene::get_instance().key_callback(window, key, scancode, action, mode);
    }
};
