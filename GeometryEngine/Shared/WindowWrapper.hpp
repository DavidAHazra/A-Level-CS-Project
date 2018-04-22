//
//  WindowWrapper.hpp
//  GeometriamMac
//
//  Created by David Hazra on 30/09/2016.
//  Copyright © 2016 BrainDeadStudios. All rights reserved.
//

#pragma once
#include "EngineHeader.hpp"


class WindowWrapper {
	// Class Constants
#ifdef IS_WINDOWS
	static const UShort WINDOW_START_X = 50;
	static const UShort WINDOW_START_Y = 50;
	static const UShort DEFAULT_WIDTH = 1080;
	static const UShort DEFAULT_HEIGHT = 720;
#else
	static const UShort WINDOW_START_X = 30;
	static const UShort WINDOW_START_Y = 100;
	static const UShort DEFAULT_WIDTH = 930;
	static const UShort DEFAULT_HEIGHT = 600;
#endif

public:
	static const unsigned char DEFAULT_MULTISAMPLE = 4;

    static WindowWrapper& get_instance(const std::string& window_title = "") {
        static WindowWrapper instance(window_title);
        return instance;
    }
    
	// Fullscreen
    inline bool get_is_fullscreen() const { return _is_fullscreen; }
    void set_fullscreen(const bool new_value);

	// Raw pointer
	inline GLFWwindow* get_window() { return _window; }

	// Wrapper functions
	inline bool is_focused() const { return is_focused(_window); }
	static inline bool is_focused(GLFWwindow* window) { return (glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE); }

	inline std::pair<UShort, UShort> get_window_dimensions() const { return get_window_dimensions(_window); }
	static std::pair<UShort, UShort> get_window_dimensions(GLFWwindow* window);

	inline std::pair<UShort, UShort> get_window_position() const { return get_window_position(_window); }
	static std::pair<UShort, UShort> get_window_position(GLFWwindow* window);
    
    inline UShort width() const { return width(_window); }
    static inline UShort width(GLFWwindow* window){ return get_window_dimensions(window).first; }
    
    inline UShort height() const { return height(_window); }
    static inline UShort height(GLFWwindow* window){ return get_window_dimensions(window).second; }

    // Multisample rate
	static int get_multisample_rate();
    
public:
    WindowWrapper(const WindowWrapper& other) = delete;
    void operator=(const WindowWrapper& other) = delete;
    
private:
    WindowWrapper(const std::string& window_title = std::string(), const UShort width = DEFAULT_WIDTH, const UShort height = DEFAULT_HEIGHT);
    
	// Instance attributes
    GLFWwindow* _window;
	std::string _window_title;
    bool _is_fullscreen = false;
};
