#include "Projectile.hpp"
#include "GameHelpers.hpp"


const UInt Potato::mana_cost = 0;
const UInt Potato::damage = 5;
const float Potato::speed = 1.0f;

const UInt FireBall::mana_cost = 20;
const UInt FireBall::damage = 0;    // Damage is done as burn damage (over time)
const float FireBall::speed = Potato::speed;


const UInt IceBall::mana_cost = 30;
const UInt IceBall::damage = 40;
const float IceBall::speed = 0.75f * Potato::speed;


const UInt Magic::mana_cost = 3;
const UInt Magic::damage = 4;
const float Magic::speed = 1.75f * Potato::speed;


Projectile::Projectile(const vec3& movement_vector, const float& speed) : projectile_cube(){
    projectile_cube.set_move_vector(normalise(movement_vector) * speed);
}

void Projectile::update(const float& time_delta){
    projectile_cube.move(projectile_cube.get_move_vector() * time_delta);
}

void Projectile::render(){
    dynamic_cast<Renderable*>(&projectile_cube)->render();
}

Potato::Potato(const vec3& movement_vector) : Projectile(movement_vector, Potato::speed),
    potato(FileSystem::get_mesh("potato/potato.obj").string()){
    potato.set_enlargement(vec3(0.5f));
}

void Potato::render() {
    potato.set_position(projectile_cube.get_position());
    potato.set_quaternion(projectile_cube.get_quaternion());
    
    dynamic_cast<Renderable*>(&potato)->render();
}

FireBall::FireBall(const vec3& movement_vector) : Projectile(movement_vector, FireBall::speed) {
    projectile_cube.set_texture(Shape::load_texture_from_file("fireball.png", FileSystem::get_textures_dir().string()));
    projectile_cube.set_enlargement(vec3(0.75f));
}

IceBall::IceBall(const vec3& movement_vector) : Projectile(movement_vector, IceBall::speed) {
    projectile_cube.set_texture(Shape::load_texture_from_rgba(0.0f, 0.81f, 0.82f));
    projectile_cube.set_texture(Shape::load_texture_from_file("iceball.png", FileSystem::get_textures_dir().string()));
    projectile_cube.set_enlargement(vec3(1.5f));
}

Magic::Magic(const vec3& movement_vector) : Projectile(movement_vector, Magic::speed) {
    projectile_cube.set_texture(Shape::load_texture_from_file("magic.png", FileSystem::get_textures_dir().string()));
    projectile_cube.set_enlargement(vec3(0.2f));
}
