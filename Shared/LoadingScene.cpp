#include "LoadingScene.hpp"
#include "ResourceHandler.hpp"
#include "GameHelpers.hpp"
#include "Shape.hpp"
#include "Camera.hpp"


LoadingScene::LoadingScene(WindowWrapper* window, Camera* camera) : BaseScene(window, camera),
loading_text(GameConstants::MECHA(), "LOADING"),
please_wait(GameConstants::MECHA(), "Please Wait") {

	bind_callbacks();
	init();

	std::pair<UInt, UInt> window_dimensions = window->get_window_dimensions();
	mat4 text_vp = ortho(0.0f, static_cast<float>(window_dimensions.first), 0.0f, static_cast<float>(window_dimensions.second));
	ResourceHandler& instance = ResourceHandler::get_instance();
	instance.get_program(Text::GENERIC_ID()).set_uniform<mat4>("VP", text_vp);
}

void LoadingScene::bind_callbacks(){
    GLFWwindow* window = this->window->get_window();
	glfwSetWindowCloseCallback(window, LoadingScene::WINDOW_CLOSE_HELPER);
}

int LoadingScene::main_loop(){
	// This scene should leave immeadiately after being rendered

	pre_render();
    render();
	post_render();

	return return_code;
}

void LoadingScene::pre_render(){
	check_errors("Main Loop Pre-Render");

    float current_frame_time = static_cast<float>(glfwGetTime());
    frame_time_delta = current_frame_time - last_frame_time;
    last_frame_time = current_frame_time;

	glfwPollEvents();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	std::pair<UShort, UShort> window_dimensions = window->get_window_dimensions();
	glViewport(0, 0, window_dimensions.first, window_dimensions.second);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LoadingScene::render(){
	dynamic_cast<Renderable*>(&loading_text)->render();
	dynamic_cast<Renderable*>(&please_wait)->render();
}

// Member Functions
void LoadingScene::init(){
	std::pair<UInt, UInt> window_dimensions = window->get_window_dimensions();
	vec3 orthogonal_screen_centre(window_dimensions.first / 2.0f, window_dimensions.second / 2.0f, 0.0f);

	loading_text.set_colour(Colour(0.4f, 0.0f, 0.4f));
	loading_text.set_to_center();
	loading_text.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y * 1.5f, 0.0f));

	please_wait.set_colour(Colour(0.8f, 0.6f, 0.8f));
	please_wait.set_scale(0.9f);
	please_wait.set_to_center();
	please_wait.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y * (2.0f / 3.0f), 0.0f));
}
