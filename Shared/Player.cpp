#include "Player.hpp"
#include "Mesh.hpp"
#include "ResourceHandler.hpp"
#include "GameHelpers.hpp"
#include "WindowWrapper.hpp"


Player::Player(const float& width, const float& height, const float& depth, Camera* camera) :
    Character(width, height, depth),
    vertical_crosshair(0.5f, 4.0f),
    horisontal_crosshair(4.0f, 0.5f),
    camera(camera),
    mana_text(FileSystem::get_font("Mecha.ttf").string(), "M"),
    health_text(FileSystem::get_font("Mecha.ttf").string(), "H"),
    score_text(FileSystem::get_font("Mecha.ttf").string(), "Score: 0"),
    potato_icon(1.0f, 1.0f),
    fireball_icon(1.0f, 1.0f),
    iceball_icon(1.0f, 1.0f),
    magic_icon(1.0f, 1.0f),
    potato_text(FileSystem::get_font("Mecha.ttf").string(), "0 mana"),
    fireball_text(FileSystem::get_font("Mecha.ttf").string(), "20 mana"),
    iceball_text(FileSystem::get_font("Mecha.ttf").string(), "30 mana"),
    magic_text(FileSystem::get_font("Mecha.ttf").string(), "15 mana"){
        
    std::pair<float, float> dimensions = WindowWrapper::get_instance().get_window_dimensions();
        
    ResourceHandler& handler = ResourceHandler::get_instance();
    handler.add_texture("potato_unselected", Shape::load_texture_from_file("potato_unselected.png", FileSystem::get_textures_dir().string()));
	handler.add_texture("potato_selected", Shape::load_texture_from_file("potato_selected.png", FileSystem::get_textures_dir().string()));
    
	handler.add_texture("fire_unselected", Shape::load_texture_from_file("fireball_unselected.png", FileSystem::get_textures_dir().string()));
	handler.add_texture("fire_selected", Shape::load_texture_from_file("fireball_selected.png", FileSystem::get_textures_dir().string()));
        
	handler.add_texture("ice_unselected", Shape::load_texture_from_file("ice_unselected.png", FileSystem::get_textures_dir().string()));
	handler.add_texture("ice_selected", Shape::load_texture_from_file("ice_selected.png", FileSystem::get_textures_dir().string()));
        
	handler.add_texture("magic_unselected", Shape::load_texture_from_file("magic_unselected.png", FileSystem::get_textures_dir().string()));
	handler.add_texture("magic_selected", Shape::load_texture_from_file("magic_selected.png", FileSystem::get_textures_dir().string()));

#ifdef IS_WINDOWS
	float text_scale = 0.2f;
	float image_scale = 75.0f;
#elif IS_APPLE
	float text_scale = 0.3f;
	float image_scale = 100.0f;
#endif

	const float icon_y = dimensions.second - 75.0f;

	potato_icon.set_texture(handler.get_texture("potato_unselected"));
    potato_icon.enlarge(image_scale);
    potato_icon.move(vec3(dimensions.first / 6.0f, icon_y, 0.0));
    potato_icon.set_texture(Shape::load_texture_from_rgba(Colours::WHITE));
    potato_text.set_position(potato_icon.get_position() - vec3(50.0f, 100.0f, 0.0f));
    potato_text.set_scale(0.2f);
    potato_text.set_colour(Colours::WHITE);
    
    fireball_icon.set_texture(handler.get_texture("fire_unselected"));
    fireball_icon.enlarge(image_scale);
    fireball_icon.move(vec3((dimensions.first / 3.0f), icon_y, 0.0));
    fireball_text.set_position(fireball_icon.get_position() - vec3(60.0f, 100.0f, 0.0f));
    fireball_text.set_scale(0.2f);

    iceball_icon.set_texture(handler.get_texture("ice_unselected"));
    iceball_icon.enlarge(image_scale);
    iceball_icon.move(vec3((dimensions.first * 4.0f / 6.0f), icon_y, 0.0));
    iceball_text.set_position(iceball_icon.get_position() - vec3(60.0f, 100.0f, 0.0f));
    iceball_text.set_scale(0.2f);

    magic_icon.set_texture(handler.get_texture("magic_unselected"));
    magic_icon.enlarge(image_scale);
    magic_icon.move(vec3(dimensions.first * 5.0f / 6.0f, icon_y, 0.0));
    magic_text.set_position(magic_icon.get_position() - vec3(60.0f, 100.0f, 0.0f));
    magic_text.set_scale(0.2f);

	float text_x = dimensions.first / 2.0f; // 5.0f;

	mana_text.set_colour(Colours::PURPLE);
	mana_text.set_scale(text_scale);
	mana_text.set_horisontal_align();
	mana_text.set_position(vec3(text_x, magic_icon.get_position().y, 0.0f));

	health_text.set_colour(Colours::CRIMSON);
	health_text.set_scale(text_scale);
	health_text.set_horisontal_align();
	health_text.set_position(vec3(text_x, magic_icon.get_position().y - ((mana_text.get_height() * 1.1f) + (health_text.get_height() * 1.1f)), 0.0f));

	score_text.set_colour(Colours::WHITE);
	score_text.set_scale(text_scale);
	score_text.set_horisontal_align();
	score_text.set_position(vec3(text_x, magic_icon.get_position().y - ((health_text.get_height() * 2.5f) + (score_text.get_height() * 1.1f)), 0.0f));

	// Crosshair
    vertical_crosshair.move(vec3(dimensions.first / 2, dimensions.second / 2, 0.0));
    horisontal_crosshair.move(vec3(dimensions.first / 2, dimensions.second / 2, 0.0));
        
#ifdef IS_WINDOWS
	const float crosshair_enlargement = 5.0f;
#elif IS_APPLE
	const float crosshair_enlargement = 10.0f;
#endif

    vertical_crosshair.enlarge(crosshair_enlargement);
    horisontal_crosshair.enlarge(crosshair_enlargement);
        
    cuboid.set_texture(Shape::load_texture_from_rgba(0.0f, 0.8f, 0.8f));

	current_ability = Abilities::MAGIC;
	next_ability();
}

Player::~Player(){
    MemoryManagement::delete_all_from_vector(projectiles);
}

void Player::move(float right, float forward){
    const vec3 camera_front = normalise(camera->get_front_vector());
    const vec3 right_vector = normalise(cross(camera_front, vec3(0.0f, 1.0f, 0.0f)));
	const vec3 forward_vector = -normalise(cross(right_vector, vec3(0.0f, 1.0f, 0.0f)));
    
    vec3 movement;    
    if (forward != 0.0f){ movement += vec3(forward_vector.x * forward, 0.0f, forward_vector.z * forward); }
    if (right != 0.0f){ movement += vec3(right_vector.x * right, 0.0f, right_vector.z * right); }
    
    cuboid.move(movement);
    
    const vec2 prev = camera->get_previous_coords();
    handle_mouse(prev.x, prev.y);
}

void Player::handle_mouse(const float& x_pos, const float& y_pos) {
    camera->handle_mouse(x_pos, y_pos);
    const vec3 cuboid_pos = cuboid.get_position();
    
    camera->set_position(cuboid.get_vertex_position(0));
    
    if (first_person){
        const vec3 new_pos(cuboid_pos.x, cuboid_pos.y * 1.25f, cuboid_pos.z);
        camera->set_position(new_pos);
        camera->set_custom_view(look_at(new_pos, new_pos + camera->get_front_vector(), vec3(0.0f, 1.0f, 0.0f)));
        
        return;
    }
    
    const float y_rotation = 270.0f - camera->get_yaw();
    
    const float camera_x = camera_distance * sin(radians(y_rotation)) + cuboid_pos.x;
    const float camera_z = camera_distance * cos(radians(y_rotation)) + cuboid_pos.z;

    const vec3 new_camera_position(camera_x, camera_distance / 2.0f, camera_z);
	camera->set_position(new_camera_position);
    camera->set_custom_view(look_at(camera->get_position(), cuboid.get_position() + vec3(0.0f, height / 2.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
}

vec3 Player::get_front_vector(){
    const vec3 camera_front = normalise(camera->get_front_vector());
    const vec3 right_vector = normalise(cross(camera_front, vec3(0.0f, 1.0f, 0.0f)));
    return -normalise(cross(right_vector, vec3(0.0f, 1.0f, 0.0f)));
}

void Player::enemy_hit() {
	health -= 10;
}

void Player::render(){
	std::pair<UInt, UInt> dim = WindowWrapper::get_instance().get_window_dimensions();
	vec3 centre(dim.first / 2.0f, dim.second / 2.0f, 0.0f);

    ResourceHandler::get_instance().get_program(Mesh::GENERIC_ID()).set_uniform<bool>("use_light_colour", false);
    if (!first_person) { dynamic_cast<Renderable*>(&cuboid)->render(); }
    
    for (size_t i = 0; i < projectiles.size(); ++i){
        projectiles.at(i)->render();
    }
    
    ResourceHandler::get_instance().get_program(Mesh::GENERIC_ID()).set_uniform<bool>("use_light_colour", true);
    
    vertical_crosshair.render("OrthoShape");
    horisontal_crosshair.render("OrthoShape");
    
    mana_text.set_text("Mana: " + std::to_string(mana));
	mana_text.set_horisontal_align();
    dynamic_cast<Renderable*>(&mana_text)->render();

    health_text.set_text("Health: " + std::to_string(health));
	health_text.set_horisontal_align();
    dynamic_cast<Renderable*>(&health_text)->render();
    
	score_text.set_horisontal_align();
    dynamic_cast<Renderable*>(&score_text)->render();
    
    dynamic_cast<Renderable*>(&potato_text)->render();
    dynamic_cast<Renderable*>(&fireball_text)->render();
    dynamic_cast<Renderable*>(&iceball_text)->render();
    dynamic_cast<Renderable*>(&magic_text)->render();
    
    potato_icon.render("OrthoShape");
    fireball_icon.render("OrthoShape");
    iceball_icon.render("OrthoShape");
    magic_icon.render("OrthoShape");
}

void Player::update(const float& time_delta){
    mana_counter += time_delta;
    if (mana_counter >= 1.0f){
        if (mana + mana_gain <= 100.0f){
            mana += mana_gain;
        }
        
        mana_counter -= 1.0f;
    }

	cuboid.set_quaternion(quat(vec3(0.0f, radians(-camera->get_yaw()), 0.0f)));
    
    for (size_t i = 0; i < projectiles.size(); ++i){
        Transformable* projectile = dynamic_cast<Transformable*>(projectiles.at(i)->get_projectile_cube());
        projectile->move_by_vector();
        projectile->rotate_all_axis(get_random<float>(0.0f, 360.0f) * time_delta, 0.0f, get_random<float>(0.0f, 360.0f) * time_delta);
    }
    
    for (size_t i = 0; i < projectiles.size(); ++i){
		Projectile* projectile = projectiles.at(i);
		vec3 pos = projectile->get_projectile_cube()->get_position();
        if ((distance(cuboid.get_position(), pos) > 300.0f) ||
			(pos.y <= -3.0f) || (pos.y >= 50.0f) || 
			(pos.x >= 100.0f) || (pos.x <= -100.0f) ||
			(pos.z >= 100.0f) || (pos.z <= -100.0f)){

            delete projectiles[i];
            projectiles[i] = nullptr;
            projectiles.erase(projectiles.begin() + i);
        }
    }

	// No need to test for potato due to 0 mana cost
	if (!Player::has_enough_for_cast(mana, Abilities::FIREBALL)) {
		fireball_text.set_colour(Colours::RED);
	} else {
		if (current_ability == Abilities::FIREBALL) { fireball_text.set_colour(Colours::WHITE); }
		else { fireball_text.set_colour(Colours::BLACK); }
	}

	if (!Player::has_enough_for_cast(mana, Abilities::ICEBALL)) {
		iceball_text.set_colour(Colours::RED);
	} else {
		if (current_ability == Abilities::ICEBALL) { iceball_text.set_colour(Colours::WHITE); }
		else { iceball_text.set_colour(Colours::BLACK); }
	}

	if (!Player::has_enough_for_cast(mana, Abilities::MAGIC)) {
		magic_text.set_colour(Colours::RED);
	} else {
		if (current_ability == Abilities::MAGIC) { magic_text.set_colour(Colours::WHITE); }
		else { magic_text.set_colour(Colours::BLACK); }
	}
}

void Player::fire_ability(){
    if (!has_enough_for_cast()) { return; }
    
    vec3 look_direction = camera->get_front_vector();
    vec3 cuboid_position = get_cuboid()->get_position();
    
    switch (current_ability){
        case (Abilities::POTATO) : {
            projectiles.push_back(new Potato(look_direction));
            projectiles.at(projectiles.size() - 1)->get_projectile_cube()->set_position(cuboid_position);
            
            break;
        }
            
        case (Abilities::FIREBALL) : {
            projectiles.push_back(new FireBall(look_direction));
            projectiles.at(projectiles.size() - 1)->get_projectile_cube()->set_position(cuboid_position);
            
            mana -= FireBall::mana_cost;

            break;
        }
            
        case (Abilities::ICEBALL) : {
            projectiles.push_back(new IceBall(look_direction));
            projectiles.at(projectiles.size() - 1)->get_projectile_cube()->set_position(cuboid_position);
            
            mana -= IceBall::mana_cost;

            break;
        }
            
        case (Abilities::MAGIC) : {
            for (size_t i = 0; i < 5; ++i){
                Magic* projectile = new Magic(look_direction);
                projectile->get_projectile_cube()->set_position(cuboid_position);
                projectile->get_projectile_cube()->move(look_direction * static_cast<float>(i));
                projectiles.push_back(projectile);
                
                mana -= Magic::mana_cost;
            }
            
            break;
        }
            
        default : { throw std::runtime_error("Projectile type not recognised"); }
    }
    
    // Don't want to handle /too/ many projectiles
    while (projectiles.size() >= 20){
        delete projectiles[0];
        projectiles[0] = nullptr;
        projectiles.erase(projectiles.begin());
    }
}

void Player::next_ability(){
    if (current_ability == Abilities::LAST_ABILITY){
        current_ability = Abilities::FIRST_ABILITY;
    } else {
        current_ability = static_cast<Abilities>(static_cast<int>(current_ability) + 1);
    }
    
    ResourceHandler* handler = &ResourceHandler::get_instance();
    
    switch (current_ability) {
        case (Abilities::POTATO): {
            magic_icon.set_texture(handler->get_texture("magic_unselected"));
            potato_icon.set_texture(handler->get_texture("potato_selected"));
            potato_text.set_colour(Colours::WHITE);
            break;
        }
            
        case (Abilities::FIREBALL): {
            potato_icon.set_texture(handler->get_texture("potato_unselected"));
            potato_text.set_colour(Colours::BLACK);
            fireball_icon.set_texture(handler->get_texture("fire_selected"));
            break;
        }
            
        case (Abilities::ICEBALL): {
            fireball_icon.set_texture(handler->get_texture("fire_unselected"));
            iceball_icon.set_texture(handler->get_texture("ice_selected"));
            break;
        }
            
        case (Abilities::MAGIC): {
            iceball_icon.set_texture(handler->get_texture("ice_unselected"));
            magic_icon.set_texture(handler->get_texture("magic_selected"));
            break;
        }
    }
}

void Player::increment_score() {
    score++;
    score_text.set_text("Score: " + std::to_string(score));
}

bool Player::has_enough_for_cast(const UInt& mana, const Abilities& ability) {
	switch (ability) {
		case Abilities::POTATO: {
			return true;    // Mana cost of the basic ability is 0, the player must have enough mana
		}

		case Abilities::FIREBALL: {
			return (mana >= FireBall::mana_cost);
		}

		case Abilities::ICEBALL: {
			return (mana >= IceBall::mana_cost);
		}

		case Abilities::MAGIC: {
			return (mana >= (Magic::mana_cost * 5.0f));
		}

		default: { throw std::runtime_error("Tried to cast an ability that has no type"); }
	}
}

