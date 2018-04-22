#include "MenuScene.hpp"
#include "ResourceHandler.hpp"
#include "GameHelpers.hpp"
#include "Shape.hpp"
#include "Camera.hpp"
#include "AttributeParser.hpp"

MenuScene* MenuScene::instance = nullptr;

MenuScene::MenuScene(WindowWrapper* window, Camera* camera) : BaseScene(window, camera),
left_column(column_width, column_length, column_width),
right_column(column_width, column_length, column_width),
top_column(column_length, column_width, column_width),
bottom_column(column_length, column_width, column_width),
background(background_dimension, background_dimension, column_width),
title(GameConstants::MECHA(), GameConstants::GAME_NAME),
play(GameConstants::MECHA(), "Play"),
options(GameConstants::MECHA(), "Options"),
exit(GameConstants::MECHA(), "Exit") {

	LightMapProgram* light_program = dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()));
	if (light_program) { light_program->reset(); }

	bind_callbacks();
	init();
	setup_framebuffer();

    Program& shape_program = ResourceHandler::get_instance().get_program(Shape::GENERIC_ID());
	shape_program.set_uniform<float>("far_plane", GameConstants::far_plane);
	shape_program.set_uniform<bool>("use_light_colour", true);
}

void MenuScene::bind_callbacks(){
    GLFWwindow* window = this->window->get_window();
    glfwSetCursorPosCallback(window, MenuScene::MOUSE_MOVEMENT_HELPER);
	glfwSetKeyCallback(window, MenuScene::KEY_CALLBACK_HELPER);
	glfwSetWindowCloseCallback(window, MenuScene::WINDOW_CLOSE_HELPER);
}

int MenuScene::main_loop(){
	while (return_code == ReturnCodes::DEFAULT){
		pre_render();
        render();
		post_render();
    }

	return return_code;
}

void MenuScene::pre_render(){
	check_errors("Main Loop Pre-Render");

    float current_frame_time = static_cast<float>(glfwGetTime());
    frame_time_delta = current_frame_time - last_frame_time;
    last_frame_time = current_frame_time;

	glfwPollEvents();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	handle_held_keys();

	LightMapProgram* light_program = dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Cube::GENERIC_ID()));
	light_program->set_uniform<bool>("use_shadows", use_shadows);

	if (using_shadows()) {
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
		ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()).set_uniform<int>("depth_map", 31);
        
        render_framebuffer();
	}

	const float minimum_value = 0.6f;
	const float red = ((1.0f - minimum_value) * std::abs(std::sin(glfwGetTime()))) + minimum_value;
    const Colour colour(red, 0.0f, 0.0f);
    
	switch (choice) {
		case (MenuChoice::PLAY) :      { play.set_colour(colour);      break; }
		case (MenuChoice::OPTIONS) :   { options.set_colour(colour);   break; }
        case (MenuChoice::EXIT) :      { exit.set_colour(colour);      break; }
	}
    
	std::pair<UShort, UShort> window_dimensions = window->get_window_dimensions();
	glViewport(0, 0, window_dimensions.first, window_dimensions.second);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (using_shadows()) { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }

	AttributeParser parser(FileSystem::join(FileSystem::get_resource_dir(), "OPTIONS").string());
	use_shadows = parser.get_attribute(Options::SHADOWS) == "On" ? true : false;
}

void MenuScene::render(){
	const std::pair<UShort, UShort> window_dimensions = window->get_window_dimensions();
	const mat4 view_matrix = camera->get_view();
	const float aspect_ratio = static_cast<float>(window_dimensions.first) / static_cast<float>(window_dimensions.second);
	const mat4 vp = perspective(GameConstants::FOV, aspect_ratio, GameConstants::near_plane, GameConstants::far_plane) * view_matrix;
    
    ResourceHandler& instance = ResourceHandler::get_instance();
	instance.get_program(Shape::GENERIC_ID()).set_uniform<mat4>("VP", vp);
	instance.get_program(Shape::GENERIC_ID()).set_uniform<vec3>("view_position", camera->get_position());
	instance.get_program("3DText").set_uniform<mat4>("VP", vp);

	for (size_t iter = 0; iter < renderables.size(); ++iter) { renderables.at(iter)->render(); }

	dynamic_cast<Renderable*>(&left_column)->render();
	dynamic_cast<Renderable*>(&right_column)->render();
	dynamic_cast<Renderable*>(&top_column)->render();
	dynamic_cast<Renderable*>(&bottom_column)->render();
	dynamic_cast<Renderable*>(&background)->render();

	title.render("3DText");
	play.render("3DText");
	options.render("3DText");
	exit.render("3DText");
	glCullFace(GL_FRONT);

	title.render("3DText");
	play.render("3DText");
	options.render("3DText");
	exit.render("3DText");
	glCullFace(GL_BACK);
}

// Member Functions
void MenuScene::handle_held_keys() {
	if (!window->is_focused()) { return; }

	// Player movement
    const float speed = 0.5f;

	const float right = ((held_keys.at(GLFW_KEY_A) ? -1.0f : 0.0f) + (held_keys.at(GLFW_KEY_D) ? 1.0f : 0.0f)) * speed;
	const float forward = ((held_keys.at(GLFW_KEY_W) ? 1.0f : 0.0f) + (held_keys.at(GLFW_KEY_S) ? -1.0f : 0.0f)) * speed;

	const vec3 camera_front = normalise(camera->get_front_vector());
	const vec3 right_vector = normalise(cross(camera_front, vec3(0.0f, 1.0f, 0.0f)));
	const vec3 forward_vector = -normalise(cross(right_vector, vec3(0.0f, 1.0f, 0.0f)));

	vec3 movement;
	if (forward != 0.0f) { movement += vec3(forward_vector.x * forward, 0.0f, forward_vector.z * forward); }
	if (right != 0.0f) { movement += vec3(right_vector.x * right, 0.0f, right_vector.z * right); }

	const vec3 new_camera_position = camera->get_position() + movement;
	camera->set_position(new_camera_position);
}

void MenuScene::init(){
	camera->set_position(vec3(0.0f, 0.0f, 10.0f));
	camera->reset();

	left_column.set_position(vec3(-15.0f, 0.0f, -35.0f));
	right_column.set_position(vec3(15.0f, 0.0f, -35.0f));
	top_column.set_position(vec3(0.0f, 15.0f, -35.0f));
	bottom_column.set_position(vec3(0.0f, -15.0f, -35.0f));

	background.set_position(vec3(0.0f, 0.0f, -50.0f));
	background.set_texture(Shape::load_texture_from_rgba(Colours::WHITE));

	LightMapProgram* light_program = dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Cube::GENERIC_ID()));
	PointLight* dynamic_lighting = new PointLight {
		vec3(0.0f, 0.0f, -10.0f),
		vec3(0.8f),
		1.0f,
		0.01f,
		0.0f,
		vec3(1.0f),
		vec3(1.0f),
		vec3(0.4f)
	};

	light_program->add_light(dynamic_lighting);

	left_column.set_texture(Shape::load_texture_from_rgba(Colours::PURPLE));
	right_column.set_texture(Shape::load_texture_from_rgba(Colours::PURPLE));
	top_column.set_texture(Shape::load_texture_from_rgba(Colours::PURPLE));
	bottom_column.set_texture(Shape::load_texture_from_rgba(Colours::PURPLE));

	title.set_scale(0.04f);
	title.set_position(vec3(0.0f, 8.0f, -35.0f));
	title.set_horisontal_align();
	title.set_colour(Colours::CRIMSON);

	play.set_scale(0.03f);
	play.set_position(vec3(0.0f, -1.0f, -35.0f));
	play.set_horisontal_align();
	play.set_colour(Colours::BLACK);

	options.set_scale(0.03f);
	options.set_position(vec3(0.0f, -7.0f, -35.0f));
	options.set_horisontal_align();
	options.set_colour(Colours::BLACK);

	exit.set_scale(0.03f);
	exit.set_position(vec3(0.0f, -13.0f, -35.0f));
    exit.set_horisontal_align();
	exit.set_colour(Colours::BLACK);
}

void MenuScene::change_choice(bool value) {
	// Value == true: Go backwards
	// Value == false: Go forwards

	switch (choice) {
		case (MenuChoice::PLAY) : {	// Start
			play.set_colour(Colours::BLACK);
			if (value) { choice = static_cast<EnumType>(static_cast<int>(choice) + 1); }
			else { choice = MenuChoice::LAST; }

			break;
		}

		case (MenuChoice::OPTIONS) : {	// Options
			options.set_colour(Colours::BLACK);
			if (value) { choice = static_cast<EnumType>(static_cast<int>(choice) + 1); }
			else { choice = static_cast<EnumType>(static_cast<int>(choice) - 1); }

			break;
		}

		case (MenuChoice::EXIT) : {	// Exit
			exit.set_colour(Colours::BLACK);
			if (value) { choice = MenuChoice::FIRST; }
			else { choice = static_cast<EnumType>(static_cast<int>(choice) - 1); }

			break;
		}
	}
}

void MenuScene::select_choice() {
	switch (choice) {
		case (MenuChoice::PLAY) :      { return_code = MainMenuReturnCodes::PLAY;		break; }
		case (MenuChoice::OPTIONS) :   { return_code = MainMenuReturnCodes::OPTIONS;    break; }
		case (MenuChoice::EXIT) :      { return_code = MainMenuReturnCodes::EXIT_GAME;  break; }
	}
}

void MenuScene::setup_framebuffer() {
	glDeleteFramebuffers(1, &framebuffer);
	glGenFramebuffers(1, &framebuffer);
	glGenTextures(1, &cube_map);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
	for (size_t face_iter = 0; face_iter < 6; ++face_iter) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<unsigned int>(face_iter),
                     0,
                     GL_DEPTH_COMPONENT,
                     GameConstants::framebuffer_depth_resolution,
                     GameConstants::framebuffer_depth_resolution,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cube_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Framebuffer is not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MenuScene::render_framebuffer() {
	if (!use_shadows) { return; }
    
    LightMapProgram* light_program = dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()));
	const vec3 light_position = light_program->get_light(0)->position;

	const std::vector<mat4> shadow_transforms = {
		GameConstants::shadow_proj * look_at(light_position, light_position + vec3(1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0)),
		GameConstants::shadow_proj * look_at(light_position, light_position + vec3(-1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0)),
		GameConstants::shadow_proj * look_at(light_position, light_position + vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)),
		GameConstants::shadow_proj * look_at(light_position, light_position + vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, -1.0)),
		GameConstants::shadow_proj * look_at(light_position, light_position + vec3(0.0, 0.0, 1.0), vec3(0.0, -1.0, 0.0)),
		GameConstants::shadow_proj * look_at(light_position, light_position + vec3(0.0, 0.0, -1.0), vec3(0.0, -1.0, 0.0))
	};

	// Render scene to depth cubemap
	Program& depth_shader = ResourceHandler::get_instance().get_program("Shadow");

	glViewport(0, 0, GameConstants::framebuffer_depth_resolution, GameConstants::framebuffer_depth_resolution);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (size_t matrix_iter = 0; matrix_iter < shadow_transforms.size(); ++matrix_iter) {
		depth_shader.set_uniform<mat4>("shadow_matrices[" + std::to_string(matrix_iter) + "]", shadow_transforms.at(matrix_iter));
	}

	depth_shader.set_uniform<float>("far_plane", GameConstants::far_plane);
	depth_shader.set_uniform<vec3>("light_position", light_position);

	for (size_t renderable_iter = 0; renderable_iter < renderables.size(); ++renderable_iter) {
		Renderable* renderable = renderables.at(renderable_iter);
		renderable->render("Shadow");
	}

	left_column.render("Shadow");
	right_column.render("Shadow");
	top_column.render("Shadow");
	bottom_column.render("Shadow");
	background.render("Shadow");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Callbacks
void MenuScene::mouse_movement(GLFWwindow* window, double x_pos, double y_pos){
	if (!this->window->is_focused()) { return; }
	camera->handle_mouse(static_cast<float>(x_pos), static_cast<float>(y_pos));
}

void MenuScene::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    static bool toggle = false;
    
	if (key == GLFW_KEY_UNKNOWN) { return; }
	else if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
		if (action == GLFW_PRESS) { held_keys.at(key) = true; }
        
        switch (key){
            case (GLFW_KEY_1) :     { toggle = !toggle;     break; }
			case (GLFW_KEY_UP) :    { change_choice(false); break; }
			case (GLFW_KEY_DOWN) :  { change_choice(true);  break; }
			case (GLFW_KEY_ENTER) : { select_choice();      break; }
        }
        
	} else if (action == GLFW_RELEASE) { held_keys.at(key) = false; }
    
    glPolygonMode(GL_FRONT_AND_BACK, toggle ? GL_LINE : GL_FILL);
}
