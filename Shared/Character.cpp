#include "Character.hpp"


Character::Character(const float& width, const float& height, const float& depth) :
	cuboid(width, height, depth),
    bounding_box(cuboid.get_personal_vertices()),
	width(width),
    height(height),
	depth(depth) {
    
    cuboid.move(vec3(0.0f, height / 2.0f, 0.0));
}

Character::Character() :
    cuboid(0.0f, 0.0f, 0.0f),
    bounding_box(cuboid.get_personal_vertices()),
    width(0.0f),
    height(0.0f),
    depth(0.0f){
}

bool Character::check_colliding(const Collidable& other) {
	return bounding_box.does_collide(dynamic_cast<Transformable*>(&cuboid)->get_model_matrix(),
									 dynamic_cast<Transformable*>(&cuboid)->get_local_axis(),
									 other.bounding_box, other.model_matrix, other.local_axis);
}
