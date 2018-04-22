#include "InstructionsScene.hpp"
#include "ResourceHandler.hpp"
#include "GameHelpers.hpp"
#include "Shape.hpp"
#include "Camera.hpp"


InstructionsScene::InstructionsScene(WindowWrapper* window, Camera* camera) : BaseScene(window, camera),
game_name(GameConstants::MECHA(), GameConstants::GAME_NAME),
movement_text(GameConstants::MECHA(), "Use WASD to move (you can move even when in menus)"),
arrow_key(GameConstants::MECHA(), "Use the arrow keys to select menu options"),
change_projectile_text(GameConstants::MECHA(), "Press R during the game to change spell for different effects"),
instructions(GameConstants::MECHA(), "Try to get as high a score as possible!"),
exit_text(GameConstants::MECHA(), "Press ENTER to continue") {

	bind_callbacks();
	init();

	std::pair<UInt, UInt> window_dimensions = window->get_window_dimensions();
	mat4 text_vp = ortho(0.0f, static_cast<float>(window_dimensions.first), 0.0f, static_cast<float>(window_dimensions.second));
	ResourceHandler& instance = ResourceHandler::get_instance();
	instance.get_program(Text::GENERIC_ID()).set_uniform<mat4>("VP", text_vp);
}

void InstructionsScene::bind_callbacks(){
    GLFWwindow* window = this->window->get_window();
	glfwSetWindowCloseCallback(window, InstructionsScene::WINDOW_CLOSE_HELPER);
    glfwSetKeyCallback(window, InstructionsScene::KEY_CALLBACK_HELPER);
}

int InstructionsScene::main_loop(){
    while (return_code == ReturnCodes::DEFAULT) {
        pre_render();
        render();
        post_render();
    }
    
	return return_code;
}

void InstructionsScene::pre_render(){
	check_errors("Main Loop Pre-Render");

    float current_frame_time = static_cast<float>(glfwGetTime());
    frame_time_delta = current_frame_time - last_frame_time;
    last_frame_time = current_frame_time;
    
	glfwPollEvents();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    const float minimum_value = 0.2f;
    const float red = ((1.0f - minimum_value) * std::abs(std::sin(glfwGetTime()))) + minimum_value;
    const Colour colour(red, 0.0f, 0.0f);

    exit_text.set_colour(colour);

	std::pair<UShort, UShort> window_dimensions = window->get_window_dimensions();
	glViewport(0, 0, window_dimensions.first, window_dimensions.second);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void InstructionsScene::render(){
	dynamic_cast<Renderable*>(&game_name)->render();
	dynamic_cast<Renderable*>(&movement_text)->render();
    dynamic_cast<Renderable*>(&arrow_key)->render();
    dynamic_cast<Renderable*>(&change_projectile_text)->render();
    dynamic_cast<Renderable*>(&instructions)->render();
    dynamic_cast<Renderable*>(&exit_text)->render();
}

// Member Functions
void InstructionsScene::init(){
	std::pair<UInt, UInt> window_dimensions = window->get_window_dimensions();
	vec3 orthogonal_screen_centre(window_dimensions.first / 2.0f, window_dimensions.second / 2.0f, 0.0f);
    
    game_name.set_colour(Colours::CRIMSON);
	game_name.set_scale(title_size);
    game_name.set_to_center();
    game_name.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y * 1.6f, 0.0f));
    
    movement_text.set_colour(Colours::WHITE);
    movement_text.set_scale(standard_size);
    movement_text.set_to_center();
    movement_text.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y * 1.2f, 0.0f));
    
    arrow_key.set_colour(Colours::WHITE);
    arrow_key.set_scale(standard_size);
    arrow_key.set_to_center();
    arrow_key.set_position(orthogonal_screen_centre);
    
    change_projectile_text.set_colour(Colours::WHITE);
    change_projectile_text.set_scale(standard_size);
    change_projectile_text.set_to_center();
    change_projectile_text.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y * 0.8f, 0.0f));
    
    instructions.set_colour(Colours::WHITE);
    instructions.set_scale(standard_size);
    instructions.set_to_center();
    instructions.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y * 0.6f, 0.0f));
    
    exit_text.set_colour(Colours::RED);
	exit_text.set_scale(title_size);
    exit_text.set_to_center();
    exit_text.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y * 0.3f, 0.0f));
}

void InstructionsScene::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
    static bool toggle = false;
    
    if (key == GLFW_KEY_UNKNOWN) { return; }
    else if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
		switch (key) {
			case (GLFW_KEY_ENTER) : { return_code = ReturnCodes::LEAVE_SCENE; break; }
		}
    }
    
    glPolygonMode(GL_FRONT_AND_BACK, toggle ? GL_LINE : GL_FILL);
}
