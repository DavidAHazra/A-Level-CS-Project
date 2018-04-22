#include "Application.hpp"
#include "ResourceHandler.hpp"
#include "Shape.hpp"
#include "SkyBox.hpp"
#include "Text.hpp"

#include "OptionsScene.hpp"
#include "LoadingScene.hpp"
#include "InstructionsScene.hpp"
#include "DeathScene.hpp"


Application::Application() {
	// Setting up game options
	AttributeParser parser(FileSystem::join(FileSystem::get_resource_dir(), "OPTIONS").string());

	if (parser.get_attribute(Options::MULTISAMPLING) == "") {
		parser.add_attribute(Options::MULTISAMPLING, std::to_string(WindowWrapper::DEFAULT_MULTISAMPLE));
	}

	if (parser.get_attribute(Options::SHADOWS) == "") { parser.add_attribute(Options::SHADOWS, "On"); }

	// Setting up rendering
	if (!glfwInit()) {
		throw std::runtime_error("GLFW failed to initialise");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, std::stoi(parser.get_attribute(Options::MULTISAMPLING)));
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

#ifdef IS_APPLE
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = &WindowWrapper::get_instance(GameConstants::GAME_NAME);

	std::string fullscreen = parser.get_attribute(Options::FULLSCREEN);
	if (fullscreen == "") { parser.add_attribute(Options::FULLSCREEN, "Off"); }
	else { window->set_fullscreen(fullscreen == "On" ? true : false); }


#ifdef IS_WINDOWS
    glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Failed to initialise GLEW");
	}
#endif

	glEnable(GL_MULTISAMPLE); // If wanting to uncomment, change window hint too!
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);	// Disables drawing of all sides (front / back)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDepthFunc(GL_LESS);

    ResourceHandler& instance = ResourceHandler::get_instance();

	instance.load_lightmap(FileSystem::get_shader("shape_light_vertex.shader").string(),
						   FileSystem::get_shader("shape_light_frag.shader").string(),
						   FileSystem::get_shader("shape_light_geo.shader").string(),
						   Shape::GENERIC_ID());
    
    instance.load_program(FileSystem::get_shader("shadow_vertex.shader").string(),
                          FileSystem::get_shader("shadow_frag.shader").string(),
                          FileSystem::get_shader("shadow_geometry.shader").string(),
                          "Shadow");
    
    instance.load_program(FileSystem::get_shader("skybox_vertex.shader").string(),
                          FileSystem::get_shader("skybox_frag.shader").string(),
                          SkyBox::GENERIC_ID());
    
    instance.load_program(FileSystem::get_shader("ortho_shape_vertex.shader").string(),
                          FileSystem::get_shader("ortho_shape_frag.shader").string(),
                          "OrthoShape");
    
    instance.load_program(FileSystem::get_shader("text_vertex.shader").string(),
                          FileSystem::get_shader("text_fragment.shader").string(),
                          Text::GENERIC_ID());

	instance.load_program(FileSystem::get_shader("text_vertex.shader").string(),
						  FileSystem::get_shader("text_fragment.shader").string(),
						  "3DText");
    
    camera = new Camera();
    
    InstructionsScene::get_instance(window, camera);
    LoadingScene::get_instance(window, camera);
}

Application::~Application() {
	glfwTerminate();
    delete camera;
}

EnumType Application::main_menu() {
	MenuScene* menu_scene = MenuScene::get_instance(window, camera);
	EnumType return_code = menu_scene->main_loop();
    MenuScene::delete_instance();
    
	return return_code;
}

std::pair<EnumType, UInt> Application::game_scene() {
	LoadingScene::get_instance().main_loop();

	GameScene* game_scene = GameScene::get_instance(window, camera);
    Attribute<UInt> player_score_getter(0);
    game_scene->set_score_getter(&player_score_getter);
    EnumType return_code = game_scene->main_loop();
    GameScene::delete_instance();
    
    return std::pair<EnumType, UInt>(return_code, player_score_getter.get_attribute());
}

EnumType Application::options_scene() {
	OptionsScene* options_scene = OptionsScene::get_instance(window, camera);
	EnumType return_code = options_scene->main_loop();
	OptionsScene::delete_instance();

	return return_code;
}

EnumType Application::death_scene(const UInt& score) {
    DeathScene* death_scene = DeathScene::get_instance(window, camera);
    death_scene->set_score(score);
    EnumType return_code = death_scene->main_loop();
    DeathScene::delete_instance();
    
    return return_code;
}

void Application::run_main_menu() {
    EnumType menu_return_code = main_menu();

	switch (menu_return_code) {
        case (MainMenuReturnCodes::EXIT_GAME) : {
			return;
		}

		case (MainMenuReturnCodes::PLAY) : {
            run_game_scene();
            break;
		}

		case (MainMenuReturnCodes::OPTIONS) : {
			run_options_scene();
			break;
		}

		default: { throw std::runtime_error("Non-standard menu return code (this should never be output)"); }
	}
}

void Application::run_game_scene() {
    std::pair<EnumType, UInt> game_return_pair = game_scene();

	switch (game_return_pair.first) {
		case (GameReturnCodes::EXIT_GAME) : {
			return;
		}

		case (GameReturnCodes::MAIN_MENU) : {
			run_main_menu();
			break;
		}
            
        case (GameReturnCodes::PLAYER_DEAD) : {
            run_death_scene(game_return_pair.second);
            break;
        }

		default: { throw std::runtime_error("Non-standard game return code (this should never be output)"); }
	}
}

void Application::run_options_scene() {
	EnumType options_return_code = options_scene();

	switch (options_return_code) {
		case (OptionsReturnCodes::MAIN_MENU) : {
			run_main_menu();
			break;
		}

		default: { throw std::runtime_error("Non-standard options return code (this should never be output)"); }
	}
}

void Application::run_death_scene(const UInt& score) {
    EnumType death_return_code = death_scene(score);
    
    switch (death_return_code) {
        case (DeathSceneReturnCodes::MAIN_MENU) : {
            run_main_menu();
            break;
        }
    }
}

void Application::start() {
    InstructionsScene::get_instance().main_loop();
    run_main_menu();
}
