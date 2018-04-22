#include "DeathScene.hpp"
#include "ResourceHandler.hpp"
#include "GameHelpers.hpp"
#include "Shape.hpp"
#include "Camera.hpp"


DeathScene* DeathScene::instance = nullptr;

DeathScene::DeathScene(WindowWrapper* window, Camera* camera) : BaseScene(window, camera),
parser(FileSystem::join(FileSystem::get_resource_dir(), "OPTIONS").string()),
left_column(column_width, column_length, column_width),
right_column(column_width, column_length, column_width),
top_column(column_length, column_width, column_width),
bottom_column(column_length, column_width, column_width),
background(background_dimension, background_dimension, column_width),
you_are_dead(GameConstants::MECHA(), "You are dead!"),
please_type(GameConstants::MECHA(), "Type to enter your username"),
score_text(GameConstants::MECHA(), "Score: 0"),
username_text(GameConstants::MECHA(), "_"),
done_text(GameConstants::MECHA(), "Continue"),
first_name(GameConstants::MECHA(), ""),
second_name(GameConstants::MECHA(), ""),
third_name(GameConstants::MECHA(), ""),
fourth_name(GameConstants::MECHA(), ""),
fifth_name(GameConstants::MECHA(), ""),
first_score(GameConstants::MECHA(), ""),
second_score(GameConstants::MECHA(), ""),
third_score(GameConstants::MECHA(), ""),
fourth_score(GameConstants::MECHA(), ""),
fifth_score(GameConstants::MECHA(), ""),
showing_top_5(GameConstants::MECHA(), "Showing top 5 scores"),
return_to_main_menu(GameConstants::MECHA(), "Return to Main Menu") {

	LightMapProgram* light_program = dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()));
	if (light_program) { light_program->reset(); }

	bind_callbacks();
	init();
	setup_framebuffer();

    Program& shape_program = ResourceHandler::get_instance().get_program(Shape::GENERIC_ID());
	shape_program.set_uniform<float>("far_plane", GameConstants::far_plane);
	shape_program.set_uniform<bool>("use_light_colour", true);
}

void DeathScene::bind_callbacks(){
    GLFWwindow* window = this->window->get_window();
    glfwSetCursorPosCallback(window, DeathScene::MOUSE_MOVEMENT_HELPER);
	glfwSetKeyCallback(window, DeathScene::KEY_CALLBACK_HELPER);
	glfwSetWindowCloseCallback(window, DeathScene::WINDOW_CLOSE_HELPER);
}

int DeathScene::main_loop(){
	while (return_code == ReturnCodes::DEFAULT){
		pre_render();
        render();
		post_render();
    }

	return return_code;
}

void DeathScene::pre_render(){
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

    if (typing) {
        switch (selection) {
            case (Selection::USERNAME) : {
                username_text.set_colour(colour);
                break;
            }
                
            case (Selection::DONE) : {
                done_text.set_colour(colour);
                break;
            }
        }
    } else {
        return_to_main_menu.set_colour(colour);
    }
    
	std::pair<UShort, UShort> window_dimensions = window->get_window_dimensions();
	glViewport(0, 0, window_dimensions.first, window_dimensions.second);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (using_shadows()) { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }

	AttributeParser parser(FileSystem::join(FileSystem::get_resource_dir(), "OPTIONS").string());
	use_shadows = parser.get_attribute(Options::SHADOWS) == "On" ? true : false;
}

void DeathScene::render(){
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
    
    you_are_dead.render("3DText");
    score_text.render("3DText");
    if (typing) {
        please_type.render("3DText");
        username_text.render("3DText");
        done_text.render("3DText");
    } else {
        first_name.render("3DText");
        second_name.render("3DText");
        third_name.render("3DText");
        fourth_name.render("3DText");
        fifth_name.render("3DText");
        
        first_score.render("3DText");
        second_score.render("3DText");
        third_score.render("3DText");
        fourth_score.render("3DText");
        fifth_score.render("3DText");
        
        showing_top_5.render("3DText");
        return_to_main_menu.render("3DText");
    }
    glCullFace(GL_FRONT);
    
    you_are_dead.render("3DText");
    score_text.render("3DText");
    if (typing) {
        please_type.render("3DText");
        username_text.render("3DText");
        done_text.render("3DText");
    } else {
        first_name.render("3DText");
        second_name.render("3DText");
        third_name.render("3DText");
        fourth_name.render("3DText");
        fifth_name.render("3DText");
        
        first_score.render("3DText");
        second_score.render("3DText");
        third_score.render("3DText");
        fourth_score.render("3DText");
        fifth_score.render("3DText");
        
        showing_top_5.render("3DText");
        return_to_main_menu.render("3DText");
    }
    glCullFace(GL_BACK);
}

// Member Functions
void DeathScene::handle_held_keys() {
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

void DeathScene::init(){
	camera->reset();
    camera->set_position(vec3(0.0f, 0.0f, 30.0f));
    
    left_column.set_position(vec3(-24.5f, 0.0f, -35.0f));
    right_column.set_position(vec3(24.5f, 0.0f, -35.0f));
    top_column.set_position(vec3(0.0f, 24.5f, -35.0f));
    bottom_column.set_position(vec3(0.0f, -24.5f, -35.0f));

	left_column.set_texture(Shape::load_texture_from_rgba(Colours::PURPLE));
	right_column.set_texture(Shape::load_texture_from_rgba(Colours::PURPLE));
	top_column.set_texture(Shape::load_texture_from_rgba(Colours::PURPLE));
	bottom_column.set_texture(Shape::load_texture_from_rgba(Colours::PURPLE));
    
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
    
    you_are_dead.set_scale(0.04f);
    you_are_dead.set_horisontal_align();
    you_are_dead.set_colour(Colours::CRIMSON);
    you_are_dead.set_position(vec3(0.0f, 18.0f, -35.0f));
    
    score_text.set_scale(0.02f);
    score_text.set_colour(Colours::BLACK);
    score_text.set_position(vec3(0.0f, 14.0f, -35.0f));

    please_type.set_scale(0.02f);
    please_type.set_horisontal_align();
    please_type.set_colour(Colours::BLACK);
    please_type.set_position(vec3(0.0f, 10.0f, -35.0f));
    
    username_text.set_scale(0.07f);
    username_text.set_horisontal_align();
    username_text.set_colour(Colours::CRIMSON);
    username_text.set_position(vec3(0.0f, -6.0f, -35.0f));
    
    done_text.set_scale(0.04f);
    done_text.set_horisontal_align();
    done_text.set_colour(Colours::BLACK);
    done_text.set_position(vec3(0.0f, -16.0f, -35.0f));
    
    showing_top_5.set_scale(0.02f);
    showing_top_5.set_horisontal_align();
    showing_top_5.set_colour(Colours::BLUE);
    showing_top_5.set_position(vec3(0.0f, 10.0f, -35.0f));
    
    return_to_main_menu.set_scale(0.035f);
    return_to_main_menu.set_horisontal_align();
    return_to_main_menu.set_colour(Colours::BLUE);
    return_to_main_menu.set_position(vec3(0.0f, bottom_column.get_position().y - 8.0f, -35.0f));
    
    // Names
    first_name.set_scale(0.035f);
    first_name.set_colour(Colours::BLACK);
    
    second_name.set_scale(0.035f);
    second_name.set_colour(Colours::BLACK);
    
    third_name.set_scale(0.035f);
    third_name.set_colour(Colours::BLACK);
    
    fourth_name.set_scale(0.035f);
    fourth_name.set_colour(Colours::BLACK);
    
    fifth_name.set_scale(0.035f);
    fifth_name.set_colour(Colours::BLACK);
    
    // Scores
    first_score.set_scale(0.035f);
    first_score.set_colour(Colours::BLACK);
    
    second_score.set_scale(0.035f);
    second_score.set_colour(Colours::BLACK);
    
    third_score.set_scale(0.035f);
    third_score.set_colour(Colours::BLACK);
    
    fourth_score.set_scale(0.035f);
    fourth_score.set_colour(Colours::BLACK);
    
    fifth_score.set_scale(0.035f);
    fifth_score.set_colour(Colours::BLACK);
}

void DeathScene::set_score(const UInt& new_score) {
    score = new_score;
    
    score_text.set_text("You scored: " + std::to_string(score));
    score_text.set_horisontal_align();
}

void DeathScene::setup_framebuffer() {
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

void DeathScene::load_stats() {
    AttributeParser leaderboard_parser(GameConstants::LEADERBOARD());
    leaderboard_parser.make_file();
    
    typedef std::map<std::string, std::string> Map;
    typedef std::pair<std::string, int> Pair;
    
    std::map<std::string, std::string> leaderboard_map = leaderboard_parser.get_attributes();
    std::map<std::string, int> scores;
    
    for (Map::const_iterator iter = leaderboard_map.begin(); iter != leaderboard_map.end(); ++iter) {
        scores.insert(Pair(iter->first, std::stoi(iter->second)));
    }
    
    std::vector<std::pair<std::string, int>> sortable_map(scores.begin(), scores.end());
    std::sort(sortable_map.begin(), sortable_map.end(), GreaterSecondPairSort<std::string, int>());
    
    const float seperator = 6.0f;
    const float first_y = 2.0f;
    const float second_y = first_y - seperator;
    const float third_y = second_y - seperator;
    const float fourth_y = third_y - seperator;
    const float fifth_y = fourth_y - seperator;
    const float name_x = left_column.get_position().x + 2.0f;
    
    // TODO: Something with this monster D:
    if (sortable_map.size() > 0){
        Pair first = sortable_map.at(0);
        first_name.set_text(first.first);
        first_score.set_text(std::to_string(first.second));
        first_name.set_position(vec3(name_x, first_y, -35.0f));
        first_score.set_position(vec3(right_column.get_position().x - first_score.get_width() - 2, first_y, -35.0f));
        
        if (sortable_map.size() > 1) {
            Pair second = sortable_map.at(1);
            second_name.set_text(second.first);
            second_score.set_text(std::to_string(second.second));
            second_name.set_position(vec3(name_x, second_y, -35.0f));
            second_score.set_position(vec3(right_column.get_position().x - second_score.get_width() - 2, second_y, -35.0f));
        
            if (sortable_map.size() > 2) {
                Pair third = sortable_map.at(2);
                third_name.set_text(third.first);
                third_score.set_text(std::to_string(third.second));
                third_name.set_position(vec3(name_x, third_y, -35.0f));
                third_score.set_position(vec3(right_column.get_position().x - third_score.get_width() - 2, third_y, -35.0f));
            
                if (sortable_map.size() > 3) {
                    Pair fourth = sortable_map.at(3);
                    fourth_name.set_text(fourth.first);
                    fourth_score.set_text(std::to_string(fourth.second));
                    fourth_name.set_position(vec3(name_x, fourth_y, -35.0f));
                    fourth_score.set_position(vec3(right_column.get_position().x - fourth_score.get_width() - 2, fourth_y, -35.0f));
                    
                    if (sortable_map.size() > 4) {
                        Pair fifth = sortable_map.at(4);
                        fifth_name.set_text(fifth.first);
                        fifth_score.set_text(std::to_string(fifth.second));
                        fifth_name.set_position(vec3(name_x, fifth_y, -35.0f));
                        fifth_score.set_position(vec3(right_column.get_position().x - fifth_score.get_width() - 2, fifth_y, -35.0f));
                    }
                }
            }
        }
    }
}

void DeathScene::render_framebuffer() {
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

void DeathScene::update_username(const char new_char){
    std::string current = username_text.get_text();
    if (current == "_") { current = ""; }
    current += new_char;
    
    username_text.set_text(current);
    username_text.set_horisontal_align();
}

// Callbacks
void DeathScene::mouse_movement(GLFWwindow* window, double x_pos, double y_pos){
	if (!this->window->is_focused()) { return; }
	camera->handle_mouse(static_cast<float>(x_pos), static_cast<float>(y_pos));
}

void DeathScene::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    static bool toggle = false;
    
	if (key == GLFW_KEY_UNKNOWN) { return; }
	else if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
        switch (key){
            case (GLFW_KEY_1) : { toggle = !toggle;     break; }
            case (GLFW_KEY_2) : { return_code = ReturnCodes::LEAVE_SCENE;  break ;}
        }
        
        if (typing){
            switch (key) {
                case (GLFW_KEY_BACKSPACE) : {
                    std::string current = username_text.get_text();
                    current = current.substr(0, current.size() - 1);
                    
                    if (current.size() == 0){
                        current = "_";
                    }
                    
                    username_text.set_text(current);
                    username_text.set_horisontal_align();
                    
                    break;
                }
                    
                case (GLFW_KEY_DOWN) : {}
                case (GLFW_KEY_UP) : {
                    switch (selection) {
                        case (Selection::USERNAME) : {
                            selection = Selection::DONE;
                            username_text.set_colour(Colours::BLACK);
                            break;
                        }
                            
                        case (Selection::DONE) : {
                            selection = Selection::USERNAME;
                            done_text.set_colour(Colours::BLACK);
                            break;
                        }
                    }
                    
                    break;
                }
                    
                case (GLFW_KEY_ENTER) : {
                    if (selection == Selection::DONE) {
                        score_text.set_text(username_text.get_text() + " scored: " + std::to_string(score));
                        score_text.set_horisontal_align();
                        typing = false;
                        
                        AttributeParser leaderboard_parser(GameConstants::LEADERBOARD());
                        if (leaderboard_parser.get_attribute(username_text.get_text()) == "") {
                            leaderboard_parser.add_attribute(username_text.get_text(), std::to_string(score));
                        } else {
                            leaderboard_parser.change_attribute(username_text.get_text(), std::to_string(score));
                        }
                        
                        load_stats();
                    }
                    
                    break;
                }
                    
                default : {
                    if (username_text.get_text().size() < max_char_count) {
                        if (GLFW_key_map.find(key) != GLFW_key_map.end()) {
                            update_username(GLFW_key_map.at(key));
                        }
                    }
                    
                    break;
                }
            }
            
        } else {
            if (action == GLFW_PRESS) { held_keys.at(key) = true; }
            
            switch (key) {
                case (GLFW_KEY_ENTER) : {
                    return_code = DeathSceneReturnCodes::MAIN_MENU;
                    break;
                }
            }
        }
        
	} else if (action == GLFW_RELEASE) { held_keys.at(key) = false; }
    
    glPolygonMode(GL_FRONT_AND_BACK, toggle ? GL_LINE : GL_FILL);
}
