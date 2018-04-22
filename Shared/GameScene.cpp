#include "GameScene.hpp"
#include "Mesh.hpp"
#include "ResourceHandler.hpp"
#include "GameHelpers.hpp"
#include "SkyBox.hpp"
#include "Camera.hpp"
#include "AttributeParser.hpp"

GameScene* GameScene::instance = nullptr;


GameScene::GameScene(WindowWrapper* window, Camera* camera) : BaseScene(window, camera),
player(player_square_dimension, player_height, player_square_dimension, camera),
sky(FileSystem::get_texture("SkyBox").string()),
terrain(FileSystem::get_mesh("Scene/ORIGINAL.obj").string()),
node_map(get_nodes()),
wave_text(GameConstants::MECHA(), ""),
pause_menu_title(GameConstants::MECHA(), "Game Paused"),
resume_text(GameConstants::MECHA(), "Resume Game"),
exit_to_main_menu(GameConstants::MECHA(), "Exit to Main Menu"),
exit_game(GameConstants::MECHA(), "Exit Game"){

	LightMapProgram* light_program = dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Cube::GENERIC_ID()));
	if (light_program) { light_program->reset(); }

	bind_callbacks();
	init();
	setup_framebuffer();

	std::pair<UInt, UInt> window_dimensions = window->get_window_dimensions();
	mat4 text_vp = ortho(0.0f, static_cast<float>(window_dimensions.first), 0.0f, static_cast<float>(window_dimensions.second));
	ResourceHandler& instance = ResourceHandler::get_instance();
	instance.get_program(Text::GENERIC_ID()).set_uniform<mat4>("VP", text_vp);
	instance.get_program("OrthoShape").set_uniform<mat4>("VP", text_vp);
	instance.get_program(Mesh::GENERIC_ID()).set_uniform<float>("far_plane", GameConstants::far_plane);
	instance.get_program(Mesh::GENERIC_ID()).set_uniform<bool>("use_light_colour", true);
}

GameScene::~GameScene(){
    MemoryManagement::delete_all_from_vector(enemies);
}

void GameScene::bind_callbacks(){
    GLFWwindow* window = this->window->get_window();
    glfwSetCursorPosCallback(window, GameScene::MOUSE_MOVEMENT_HELPER);
	glfwSetKeyCallback(window, GameScene::KEY_CALLBACK_HELPER);
	glfwSetWindowCloseCallback(window, GameScene::WINDOW_CLOSE_HELPER);
    glfwSetMouseButtonCallback(window, GameScene::MOUSE_KEY_HELPER);
}

int GameScene::main_loop(){
    while (return_code == ReturnCodes::DEFAULT){
		pre_render();
        render();
		post_render();
    }
    
    player_score_getter->set_attribute(player.get_score());
    
    return return_code;
}

void GameScene::pre_render(){
	check_errors("Main Loop Pre-Render");

    float current_frame_time = static_cast<float>(glfwGetTime());
    frame_time_delta = current_frame_time - last_frame_time;
    last_frame_time = current_frame_time;

	wave_text_timer += frame_time_delta;

	LightMapProgram* light_program = dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()));
    light_program->set_uniform<bool>("use_shadows", use_shadows);

	if (using_shadows()) {
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
		ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()).set_uniform<int>("depth_map", 31);

		render_framebuffer();
	}

	glfwPollEvents();
    
    if (player.get_health() <= 0) {
        return_code = GameReturnCodes::PLAYER_DEAD;
    }
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    sky.rotate_x(frame_time_delta * 4.0f);
	sun.rotate_all_axis(frame_time_delta * 10.0f, 0.0f, frame_time_delta * 10.0f);
    
	if (!is_paused()) {
		handle_held_keys();

		if (enemies.empty()) {
			wave_text_timer = 0.0f;
			spawn_wave();
		}

		player.update(frame_time_delta);
		set_enemy_pathfind();
		evaluate_enemy_collisions();
		evaluate_player_collisions();
		update_enemies();

	} else {
		if (pause_activated) {

		} else {
			wave_text_timer = 0.0f;
			wave_text.set_text("You are dead!");
		}
	}
    
	std::pair<UInt, UInt> window_dimensions = window->get_window_dimensions();
	glViewport(0, 0, window_dimensions.first, window_dimensions.second);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (using_shadows()) { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }

	AttributeParser parser(FileSystem::join(FileSystem::get_resource_dir(), "OPTIONS").string());
	use_shadows = parser.get_attribute(Options::SHADOWS) == "On" ? true : false;
}

void GameScene::render(){
	std::pair<float, float> window_dimensions = window->get_window_dimensions();
    mat4 view_matrix = camera->get_custom_view();
	GLfloat aspect_ratio = window_dimensions.first / window_dimensions.second;

    mat4 vp =  perspective(GameConstants::FOV, aspect_ratio, GameConstants::near_plane, GameConstants::far_plane) * view_matrix;

	ResourceHandler& instance = ResourceHandler::get_instance();
	instance.get_program(Mesh::GENERIC_ID()).set_uniform<mat4>("VP", vp);
	instance.get_program(Mesh::GENERIC_ID()).set_uniform<vec3>("view_position", camera->get_position());
	instance.get_program(SkyBox::GENERIC_ID()).set_uniform<mat4>("VP", vp);
	instance.get_program("3DText").set_uniform<mat4>("VP", vp);

	if (pause_activated) {
		const float minimum_value = 0.2f;
		float sin_colour = ((1.0f - minimum_value) * std::abs(std::sin(glfwGetTime()))) + minimum_value;
		Colour colour(sin_colour);

		switch (selected_option) {
			case (PauseMenuChoices::RESUME) :		{ resume_text.set_colour(colour);		break; }
			case (PauseMenuChoices::MAIN_MENU) :	{ exit_to_main_menu.set_colour(colour); break; }
			case (PauseMenuChoices::EXIT_GAME) :	{ exit_game.set_colour(colour);			break; }
		}

		// Text is rendered orthogonally by default
		dynamic_cast<Renderable*>(&pause_menu_title)->render();
		dynamic_cast<Renderable*>(&resume_text)->render();
		dynamic_cast<Renderable*>(&exit_to_main_menu)->render();
		dynamic_cast<Renderable*>(&exit_game)->render();

	} else {
		for (size_t renderable_iter = 0; renderable_iter < renderables.size(); ++renderable_iter) {
			Renderable* renderable = renderables.at(renderable_iter);
			renderable->render();
		}

		dynamic_cast<Renderable*>(&terrain)->render();
		dynamic_cast<Renderable*>(&sky)->render();
		dynamic_cast<Renderable*>(&sun)->render();

		player.render();

		for (size_t enemy_iter = 0; enemy_iter < enemies.size(); ++enemy_iter) {
			enemies.at(enemy_iter)->rotate_text_towards_position(camera->get_position());
			enemies.at(enemy_iter)->render();
		}

		display_wave_text();
	}
}

// Member Functions
void GameScene::handle_held_keys() {
	if (!window->is_focused()) { return; }
    
    // Player movement
    player.set_speed(0.5f);
    
    const float right =     ((held_keys.at(GLFW_KEY_A) ? -1.0f : 0.0f) + (held_keys.at(GLFW_KEY_D) ? 1.0f : 0.0f)) * player.get_speed();
    const float forward =   ((held_keys.at(GLFW_KEY_W) ? 1.0f : 0.0f) + (held_keys.at(GLFW_KEY_S) ? -1.0f : 0.0f)) * player.get_speed();
    
    player.move(right, forward);
}

void GameScene::setup_framebuffer() {
    glGenFramebuffers(1, &framebuffer);
    glGenTextures(1, &cube_map);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
    for (size_t face_iter = 0; face_iter < 6; ++face_iter){
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

void GameScene::render_framebuffer() {
    if (!use_shadows) { return; }
    LightMapProgram* light_program = dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()));
    vec3 light_position = light_program->get_light(0)->position;
    
    std::vector<mat4> shadow_transforms = {
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
    
    for (size_t matrix_iter = 0; matrix_iter < shadow_transforms.size(); ++matrix_iter){
        depth_shader.set_uniform<mat4>("shadow_matrices[" + std::to_string(matrix_iter) + "]", shadow_transforms.at(matrix_iter));
    }
    
    depth_shader.set_uniform<float>("far_plane", GameConstants::far_plane);
    depth_shader.set_uniform<vec3>("light_position", light_position);
    
    for (size_t renderable_iter = 0; renderable_iter < renderables.size(); ++renderable_iter){
        Renderable* renderable = renderables.at(renderable_iter);
        renderable->render("Shadow");
    }
    
    terrain.render("Shadow");
    player.get_cuboid()->render("Shadow");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameScene::init(){
    camera->set_position(vec3(0.0f, 5.0f, 20.0f));
    sky.enlarge(GameConstants::far_plane * 0.75f);

	float sun_pos = 25.0f;
    
    LightMapProgram* light_program = dynamic_cast<LightMapProgram*>(&ResourceHandler::get_instance().get_program(Shape::GENERIC_ID()));
    
    PointLight* praise_the_sun = new PointLight {
        vec3(0.0f, sun_pos, 0.0f),
        vec3(0.8f),
        1.0f,
        0.01f,
        0.0f,
        vec3(1.0f),
        vec3(1.0f),
        vec3(0.4f)
    };
    
    light_program->add_light(praise_the_sun);
    
	// Game
	sun.set_texture(Shape::load_texture_from_file("light.png", FileSystem::get_mesh("Scene").string()));
	sun.set_position(vec3(0.0f, sun_pos, 0.0f));
	sun.enlarge(4.0f);

	wave_text.set_scale(0.6f);
	wave_text.set_colour(Colours::WHITE);

	// Menu
	std::pair<UInt, UInt> window_dimensions = window->get_window_dimensions();
	vec3 orthogonal_screen_centre(window_dimensions.first / 2.0f, window_dimensions.second / 2.0f, 0.0f);

	const float title_text = 0.5f;
	const float options_text = 0.3f;

	pause_menu_title.set_colour(Colours::CRIMSON);
	pause_menu_title.set_scale(title_text);
	pause_menu_title.set_to_center();
	pause_menu_title.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y * 3.0f / 2.0f, 0.0f));

	resume_text.set_colour(Colours::BLUE);
	resume_text.set_scale(options_text);
	resume_text.set_to_center();
	resume_text.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y, 0.0f));

	exit_to_main_menu.set_colour(Colours::BLUE);
	exit_to_main_menu.set_scale(options_text);
	exit_to_main_menu.set_to_center();
	exit_to_main_menu.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y * 0.75f, 0.0f));

	exit_game.set_colour(Colours::BLUE);
	exit_game.set_scale(options_text);
	exit_game.set_to_center();
	exit_game.set_position(vec3(orthogonal_screen_centre.x, orthogonal_screen_centre.y / 2.0f, 0.0f));
}

void GameScene::change_selection(bool up) {
	const Colour text_colour = Colours::BLUE;

	switch (selected_option) {
		case (PauseMenuChoices::RESUME) : {
			resume_text.set_colour(text_colour);
			if (up) { selected_option = PauseMenuChoices::LAST; }
			else { selected_option = static_cast<EnumType>(static_cast<int>(selected_option) + 1); }

			break;
		}

		case (PauseMenuChoices::MAIN_MENU) : {
			exit_to_main_menu.set_colour(text_colour);
			if (up) { selected_option = static_cast<EnumType>(static_cast<int>(selected_option) - 1); }
			else { selected_option = static_cast<EnumType>(static_cast<int>(selected_option) + 1); }

			break;
		}

		case (PauseMenuChoices::EXIT_GAME) : {
			exit_game.set_colour(text_colour);
			if (up) { selected_option = static_cast<EnumType>(static_cast<int>(selected_option) - 1); }
			else { selected_option = PauseMenuChoices::FIRST; }

			break;
		}
	}
}

void GameScene::display_wave_text() {
	if (wave_text_timer < 4.5f) {
		std::pair<UInt, UInt> window_dimensions = window->get_window_dimensions();
		
        wave_text.set_text("Wave: " + std::to_string(wave_number - 1));

		float half_text_width = wave_text.get_width() / 2.0f;

		wave_text.set_x((window_dimensions.first / 2.0f) - half_text_width);
		wave_text.set_y((window_dimensions.second / 2.0f) - half_text_width);
		dynamic_cast<Renderable*>(&wave_text)->render();
	}
}

std::vector<std::vector<Node*>> GameScene::get_nodes(){
    std::vector<std::vector<Node*>> node_array;
    
    const std::vector<vec2> wall_blocks = {
		// TODO: Do something with this (old content)
    };

    for (int x = x_lower_bound; x <= x_upper_including; x += 2){
        std::vector<Node*> current_row;
        
        for (int z = z_lower_bound; z <= z_upper_including; z += 2){
            Node* new_node = new Node(x, z);
            
            //Cube* cube = new Cube;
            //cube->set_position(vec3(x, 2.0f, z));
            //renderables.push_back(cube);
            
            if ((x <= x_lower_bound)  ||
                (x >= x_upper_including)   ||
                (z <= z_lower_bound)  ||
                (z >= z_upper_including)   ||
                (std::find(wall_blocks.begin(), wall_blocks.end(), vec2(static_cast<float>(x), static_cast<float>(z))) != wall_blocks.end())) {
                
                new_node->set_wall(true);
                //cube->set_texture(Shape::load_texture_from_rgba(Colours::BLUE));
            }

            current_row.push_back(new_node);
            
        }
        
        node_array.push_back(current_row);
    }
    
    return node_array;
}

void GameScene::spawn_wave() {
	// _ used as iterator because it is not used in the loop
	for (size_t _ = 0; _ < wave_number; ++_) {
		spawn_enemy();
	}

	wave_number++;
}

void GameScene::spawn_enemy(){
    vec3 position(
		get_random<float>(-80.0f, 80.0f),
		0.0f,
		get_random<float>(-80.0f, 80.0f)
	);
    
    Enemy* new_enemy = new Enemy;
    new_enemy->get_cuboid()->move(position);   
	new_enemy->set_shortest_path(node_map.a_star_pathfind(node_map.get_closest_node(position), node_map.get_closest_node(player.get_cuboid()->get_position())));
    enemies.push_back(new_enemy);
}

void GameScene::evaluate_enemy_collisions(){
	for (size_t enemy_iter = 0; enemy_iter < enemies.size(); ++enemy_iter) {	
		Enemy* current_enemy = enemies.at(enemy_iter);
		vec3 pos = current_enemy->get_cuboid()->get_position();
		if (current_enemy->get_is_done() || current_enemy->get_is_exploding()) { continue; }

		if ((pos.x > x_upper_collision) ||
			(pos.x < x_lower_collision) ||
			(pos.z > z_upper_collision) ||
			(pos.z < z_lower_collision)) {
            
            current_enemy->get_cuboid()->move(vec3(-pos.x, 0.0f, -pos.z));
            current_enemy->get_cuboid()->move(vec3(get_random<float>(-80.0f, 80.0f),
                                                            0.0f,
                                                        get_random<float>(-80.0f, 80.0f)
                                                        ));
			continue;
		}

		for (size_t projectile_iter = 0; projectile_iter < player.get_projectiles().size(); ++projectile_iter){
			Projectile* current_projectile = player.get_projectiles().at(projectile_iter);

			if (current_enemy->check_colliding(current_projectile->get_collidable())) {
				current_enemy->hit(current_projectile->get_type());
                
                if (current_projectile->get_type() == Abilities::MAGIC) {
                    vec3 direction = normalise(current_projectile->get_projectile_cube()->get_move_vector());
                    
                    current_enemy->get_cuboid()->move(vec3(direction.x, 0.0f, direction.z) * 2.0f);
                }

				delete player.get_projectiles()[projectile_iter];
				player.get_projectiles()[projectile_iter] = nullptr;
				player.get_projectiles().erase(player.get_projectiles().begin() + projectile_iter);
			}
		}
    }
}

void GameScene::evaluate_player_collisions() {
	Cuboid* player_cuboid = player.get_cuboid();
	vec3 player_pos = player_cuboid->get_position();

	if (player_pos.x > x_upper_collision) {
		player_cuboid->set_position(vec3(x_upper_collision, player_pos.y, player_pos.z));
	}

	if (player_pos.x < x_lower_collision) {
		player_cuboid->set_position(vec3(x_lower_collision, player_pos.y, player_pos.z));
	}

	// Must be redefined so z collisions don't use data set during x collisions
	player_pos = player_cuboid->get_position();
	if (player_pos.z > z_upper_collision) {
		player_cuboid->set_position(vec3(player_pos.x, player_pos.y, z_upper_collision));
	}

	if (player_pos.z < z_lower_collision) {
		player_cuboid->set_position(vec3(player_pos.x, player_pos.y, z_lower_collision));
	}

	for (size_t i = 0; i < enemies.size(); ++i) {
		Enemy* enemy = enemies.at(i);
        if (enemy->get_is_exploding()) { continue; }
		Collidable collidable = Collidable::make_collidable(*enemy->get_cuboid());

		if (player.check_colliding(collidable)) {
			player.enemy_hit();

			delete enemies[i];
			enemies[i] = nullptr;
			enemies.erase(enemies.begin() + i);
		}
	}
}

void GameScene::set_enemy_pathfind(){
    Node* player_node = node_map.get_closest_node(player.get_cuboid()->get_position());

    for (size_t i = 0; i < enemies.size(); ++i){
        Enemy* enemy = enemies.at(i);
        
        if (enemy->get_path_counter() >= Enemy::PATH_REFRESH_TIME){
			Node* enemy_node = node_map.get_closest_node(enemy->get_cuboid()->get_position());
			auto path = node_map.a_star_pathfind(enemy_node, player_node);
			enemy->set_shortest_path(path);
			//pathfind_test(path);
        }
    }
}

void GameScene::pathfind_test(const std::vector<Node*>& path) {
	if (!path.empty()) {
		for (size_t i = 0; i < path.size(); ++i) {
			Node* node = path.at(i);

			for (size_t j = 0; j < renderables.size(); ++j) {
				Cube* cube = dynamic_cast<Cube*>(renderables.at(j));
				if (!cube) { continue; }

				vec3 cube_pos = cube->get_position();
				if ((cube_pos.x == node->get_x()) && (cube_pos.z == node->get_y())) {
					cube->set_texture(Shape::load_texture_from_rgba(0, 1, 0));
				}
			}
		}
	}
}

void GameScene::select_node() {
	vec3 player_pos = player.get_cuboid()->get_position();
	vec2 player_coord = { player_pos.x, player_pos.z };

	Node* closest = node_map.get_closest_node(player_pos);

	for (size_t iter = 0; iter < renderables.size(); ++iter) {
		Cube* cube = dynamic_cast<Cube*>(renderables.at(iter));
		if (cube) {
			vec3 cube_pos = cube->get_position();
			if ((cube_pos.x == closest->get_x()) && (cube_pos.z == closest->get_y())) {
				cube->set_texture(Shape::load_texture_from_rgba(1, 0, 0));
				std::cout << "{ " << cube_pos.x << ", " << cube_pos.z << " }," << std::endl;
			}
		}
	}
}

void GameScene::update_enemies() {
    for (size_t enemy_iter = 0; enemy_iter < enemies.size(); ++enemy_iter){
        Enemy* enemy = enemies.at(enemy_iter);

        if (enemy->get_is_done()){
            player.increment_score();
            
            delete enemies[enemy_iter];
            enemies[enemy_iter] = nullptr;
            enemies.erase(enemies.begin() + enemy_iter);

            enemy_iter = 0;
            continue;
        }
        
		enemy->move_towards_closest_node(node_map);
		enemy->update(frame_time_delta);
    }
}

void GameScene::select_menu_item() {
	switch (selected_option) {
		case (PauseMenuChoices::RESUME) :		{ pause_activated = false;						break; }
		case (PauseMenuChoices::MAIN_MENU) :	{ return_code = GameReturnCodes::MAIN_MENU;		break; }
		case (PauseMenuChoices::EXIT_GAME) :	{ return_code = GameReturnCodes::EXIT_GAME;		break; }
	}
}

// Callbacks
void GameScene::mouse_movement(GLFWwindow* window, double x_pos, double y_pos){
	if (!this->window->is_focused()) { return; }
	player.handle_mouse(static_cast<float>(x_pos), static_cast<float>(y_pos));
}

void GameScene::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    static bool toggle = false;

	if (key == GLFW_KEY_UNKNOWN) { return; }
	else if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
		if (action == GLFW_PRESS) { held_keys.at(key) = true; }
        
        switch (key){
            case (GLFW_KEY_F) : { player.toggle_first_person(); break; }
            case (GLFW_KEY_R) : { player.next_ability();		break; }
            case (GLFW_KEY_1) : { toggle = !toggle;				break; }

			// Pause menu
			case (GLFW_KEY_ESCAPE) :	{ pause_activated = !pause_activated;			break; }
			case (GLFW_KEY_ENTER) :		{ if (pause_activated) select_menu_item();		break; }
			case (GLFW_KEY_UP) :		{ if (pause_activated) change_selection(true);	break; }
			case (GLFW_KEY_DOWN) :		{ if (pause_activated) change_selection(false); break; }
        }
        
	} else if (action == GLFW_RELEASE) { held_keys.at(key) = false; }
    
    glPolygonMode(GL_FRONT_AND_BACK, toggle ? GL_LINE : GL_FILL);
}

void GameScene::mouse_button(GLFWwindow* window, int button, int action, int mods){
	if (action == GLFW_PRESS) {
		held_mouse_keys.at(button) = true;

        switch (button){
			case (GLFW_MOUSE_BUTTON_LEFT) : { if (!is_paused()) { player.fire_ability(); } break; }
		}
	
    } else if (action == GLFW_RELEASE) { held_mouse_keys.at(button) = false; }
}
