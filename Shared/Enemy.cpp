#include "Enemy.hpp"
#include "Model.hpp"
#include "ResourceHandler.hpp"
#include "GameHelpers.hpp"

const float Enemy::PATH_REFRESH_TIME = 2.5f;

Enemy::Enemy() : Character(),
health_text(FileSystem::get_font("Mecha.ttf").string(), "100"),
renderable(&cuboid) {

	set_renderable(new Model(FileSystem::get_mesh("Enemy/icosphere.obj").string()), true);

	float height = bounding_box.get_aabb_max().y;
	float width = bounding_box.get_aabb_max().x;
	float depth = bounding_box.get_aabb_max().z;

	cuboid = Cuboid(width * 2, height * 2, depth * 2);
	cuboid.move(vec3(0.0f, bounding_box.get_aabb_max().y * 2.0f, 0.0f));

	health_text.set_scale(0.01f);
	health_text.set_colour(Colours::GREEN);
    health_text.set_horisontal_align();
	health_text.set_position(cuboid.get_position() + vec3(0.0f, 1.0f, 0.0f));
    
    MeshTexture new_texture = {
        MeshTexture::TEXTURE,
        static_cast<int>(Shape::load_texture_from_rgba(Colours::WHITE)),
        "texture_diffuse"
    };

}

Enemy::~Enemy(){
    if (dynamic){
        delete renderable;
        renderable = nullptr;
    }
}

void Enemy::update(const float& time_delta){
    if (do_explode){
        time_exploding += time_delta;
        
        if (time_exploding >= 2.0f){ is_done = true; }
    }
    
	if (path_counter >= PATH_REFRESH_TIME) {
		path_counter = 0.0f;
	}
    
    ice_timer += time_delta;
    fire_timer += time_delta;
    
    if (fire_timer < burn_timeout) {
        health -= 1;
    }

    path_counter += time_delta;
    const float speed = (ice_timer < ice_timeout) ? 1.0f : 5.0f;
    if (!do_explode) { cuboid.move(target * time_delta * speed); }
    
    if (health <= 0) {
        health_text.set_text("0");
        explode();
        return;
    }
    
    if (health > 75) {
        health_text.set_colour(Colours::GREEN);
    }
    else if (health > 50) {
        health_text.set_colour(Colours::YELLOW);
    }
    else if (health > 25) {
        health_text.set_colour(Colours::ORANGE);
    }
    else {
        health_text.set_colour(Colours::RED);
    }
    
    health_text.set_text(std::to_string(static_cast<int>(health)));
    health_text.set_horisontal_align();
}

void Enemy::set_renderable(Renderable* new_renderable, bool dynamic_object) {
	renderable = new_renderable;
	dynamic = dynamic_object;

	bounding_box = SAT_OBB(dynamic_cast<Shape*>(renderable)->get_personal_vertices());
}

void Enemy::render() {
	if (do_explode) {
		ResourceHandler::get_instance().get_program(Mesh::GENERIC_ID()).set_uniform<bool>("do_explode", true);
		ResourceHandler::get_instance().get_program(Mesh::GENERIC_ID()).set_uniform<float>("time", time_exploding);

		glCullFace(GL_FRONT);
	}

	Transformable* trans = dynamic_cast<Transformable*>(renderable);
	trans->set_position(cuboid.get_position());
    trans->set_quaternion(cuboid.get_quaternion());
    trans->set_enlargement(cuboid.get_enlargement());
	trans->set_rotation_point(cuboid.get_rotation_point());

    renderable->render();
    //dynamic_cast<Renderable*>(&cuboid)->render();
    
    if (do_explode){
        glCullFace(GL_BACK);
		renderable->render();
		ResourceHandler::get_instance().get_program(Mesh::GENERIC_ID()).set_uniform<bool>("do_explode", false);
    }

	health_text.set_position(cuboid.get_position() + vec3(0.0f, 1.0f, 0.0f));
	health_text.render("3DText");
}

void Enemy::rotate_text_towards_position(const vec3& pos) {
	vec3 dir = pos - cuboid.get_position();

	quat desired_quat = look_at(dir, vec3(0.0f, 1.0f, 0.0f));
	quat quat_rotation = SLERP(health_text.get_quaternion(), desired_quat, 0.5f);

	health_text.set_quaternion(quat_rotation);
}

void Enemy::hit(const Abilities& projectile_type) {
    MeshTexture new_texture = {
        MeshTexture::TEXTURE,
        -1,
        "texture_diffuse"
    };
    
    Model* model = dynamic_cast<Model*>(renderable);

    
	switch (projectile_type) {
		case (Abilities::POTATO) : {
			health -= Potato::damage;
            new_texture.id = Shape::load_texture_from_rgba(Colours::WHITE);
            
            if (model) {
                model->meshes.at(0).set_textures({ new_texture });
            }
            
			break;
		}

		case (Abilities::FIREBALL) : {
			health -= FireBall::damage;
            new_texture.id = Shape::load_texture_from_rgba(Colour(0.61f, 0.17f, 0.17f));
            
            if (model) {
                model->meshes.at(0).set_textures({ new_texture });
            }
            
            fire_timer = 0.0f;
            ice_timer = ice_timeout;
            
			break;
		}

		case (Abilities::ICEBALL) : {
			health -= IceBall::damage;
            new_texture.id = Shape::load_texture_from_rgba(Colour(0.83f, 0.94f, 1.0f));
            
            if (model) {
                model->meshes.at(0).set_textures({ new_texture });
            }
            
            ice_timer = 0.0f;
            fire_timer = burn_timeout;
            
			break;
		}

		case (Abilities::MAGIC) : {
			health -= Magic::damage;
            
			break;
		}

		default: { throw std::runtime_error("Enemy was hit by illegal projectile"); }
	}
}

void Enemy::move_towards_closest_node(Map& map){
    if (path.size() <= 1) {
        target = vec3(0.0f);
        
    } else {
		Node* next_node = get_next_node(map);
		//Node* next_node = path.at(0);

		vec3 next_node_pos(next_node->get_x(), cuboid.get_position().y, next_node->get_y());

		target = normalise(next_node_pos - cuboid.get_position());
    }
}

Node* Enemy::get_next_node(Map& map) {
	Node* closest = map.get_closest_node(get_cuboid()->get_position());

	for (size_t i = 0; i < path.size(); ++i) {
		Node* path_node = path.at(i);
		if (path_node == closest) {

			if ((i + 1) >= path.size()) {
				return path_node;
			}

			Node* return_node = path.at(i + 1);

			return return_node;
		}
	}

	return closest;
}
