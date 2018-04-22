#pragma once
#include "BaseScene.hpp"
#include "Cuboid.hpp"
#include "Text.hpp"
#include "WindowWrapper.hpp"


class LoadingScene : public BaseScene {

public:
    // Constructors and Destructors
	LoadingScene(WindowWrapper* window, Camera* camera);
    virtual ~LoadingScene() {}

    // Deleted Versions
	LoadingScene(const LoadingScene& other) = delete;
    void operator=(const LoadingScene& other) = delete;
    
    // Instance Management
    static LoadingScene& get_instance(WindowWrapper* window = nullptr, Camera* camera = nullptr){
		static LoadingScene instance((!window ? &WindowWrapper::get_instance() : window), camera);
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
	Text loading_text;
	Text please_wait;

    // Private Member Functions
    void init();

public:
    // Callback functions
    inline void window_close(GLFWwindow* window) { return_code = ReturnCodes::EXIT_GAME; }

	static inline void WINDOW_CLOSE_HELPER(GLFWwindow* window) {
		LoadingScene::get_instance().window_close(window);
	}
};
