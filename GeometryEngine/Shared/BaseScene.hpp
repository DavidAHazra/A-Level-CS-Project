#pragma once
#include "EngineHeader.hpp"
#include "Renderable.hpp"

class WindowWrapper;
class Camera;


class ReturnCodes {
public:
    // Class heirarchy set up so that derived classes can access base elements
    // All return codes must be UNIQUE
    
    static const EnumType LEAVE_SCENE = -1;
    static const EnumType DEFAULT = 0;
    static const EnumType EXIT_GAME = 1;
};


class BaseScene {
public:
    BaseScene(WindowWrapper* window, Camera* camera);
	virtual ~BaseScene();

	virtual int main_loop() = 0;    // Returns return_code
        
protected:
    EnumType return_code = ReturnCodes::DEFAULT;
    WindowWrapper* window;
    Camera* camera;
	std::vector<Renderable*> renderables;
	std::array<bool, 1024> held_keys;
	std::array<bool, GLFW_MOUSE_BUTTON_LAST> held_mouse_keys;
    
    // Frame changes  
    float frame_time_delta;
	float last_frame_time;
    
    // Callbacks and functions
    virtual void bind_callbacks() = 0;

	virtual void pre_render() = 0;
	virtual void render() = 0;
	virtual void post_render() = 0;
};

 
