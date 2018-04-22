#include "WindowWrapper.hpp"

WindowWrapper::WindowWrapper(const std::string& window_title, const UShort width, const UShort height)
	: _window_title(window_title), _window(glfwCreateWindow(width, height, window_title.c_str(), nullptr, nullptr)){

    if (!_window) {
		throw std::runtime_error("GLFW failed to create a window");
    }

	glfwSetWindowPos(_window, WINDOW_START_X, WINDOW_START_Y);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwMakeContextCurrent(_window);
}

void WindowWrapper::set_fullscreen(const bool new_value){
    _is_fullscreen = new_value;
    
    if (_is_fullscreen){
        const GLFWvidmode* video_mode = glfwGetVideoMode(glfwGetWindowMonitor(_window) ? glfwGetWindowMonitor(_window) : glfwGetPrimaryMonitor());
        glfwSetWindowPos(_window, 0, 0);

#ifdef IS_WINDOWS		
		RECT available_area;
		bool success = SystemParametersInfo(SPI_GETWORKAREA, 0, &available_area, 0) == 0 ? false : true;
		glfwSetWindowSize(_window, success ? available_area.right : video_mode->width, success ? available_area.bottom : video_mode->height);
#elif defined(IS_APPLE)
		glfwSetWindowSize(_window, video_mode->width, video_mode->height);
        
#endif

    } else {
        glfwSetWindowSize(_window, DEFAULT_WIDTH, DEFAULT_HEIGHT);
        glfwSetWindowPos(_window, WINDOW_START_X, WINDOW_START_Y);
    }
    
    glViewport(0, 0, width(), height());

}

std::pair<UShort, UShort> WindowWrapper::get_window_dimensions(GLFWwindow* window) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
    
#ifdef IS_APPLE
    width *= 2;
    height *= 2;
#endif
    
	return std::pair<UShort, UShort>(static_cast<UShort>(width), static_cast<UShort>(height));
}

std::pair<UShort, UShort> WindowWrapper::get_window_position(GLFWwindow* window) {
	int x, y;
	glfwGetWindowPos(window, &x, &y);
	return std::pair<UShort, UShort>(static_cast<UShort>(x), static_cast<UShort>(y));
}

int WindowWrapper::get_multisample_rate() {
	int value;
	glGetIntegerv(GL_SAMPLES, &value);
	return value;
}
