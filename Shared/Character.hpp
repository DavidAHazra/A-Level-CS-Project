#pragma once
#include "Cuboid.hpp"
#include "SAT_OBB.hpp"


class Character {    
public:
	Character(const Character& other) = delete;
    void operator=(const Character& other) = delete;
    
public:
	Character(const float& width, const float& height, const float& depth);
    virtual ~Character(){}
    
    virtual void update(const float& time_delta) = 0;
    virtual void render() = 0;
    
protected:
    Character();
	Cuboid cuboid;
	SAT_OBB bounding_box;
       
	float width;
    float height;
	float depth;
    
    float speed = 2.0f;
        
public:
    // Inline getters / setters
    
    inline Cuboid* get_cuboid() { return &cuboid; }
            
    inline float get_width() { return width; }
    inline float get_depth() { return depth; }
    inline float get_height() { return height; }
    
    inline void set_speed(const float& new_speed) { speed = new_speed; }
    inline float get_speed() { return speed; }
	bool check_colliding(const Collidable& other);
};

