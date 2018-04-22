#pragma once

#include "EngineHeader.hpp"
#include "Cube.hpp"
#include "SAT_OBB.hpp"

class Cuboid : public Cube {
    /*
        TODO: Something about this class due to explaination
     
        Here, I know that a cuboid 'is not a Cube' and therefore should not inherit from it;
        however, in this context, the Cube class simply represents a renderable set of 8 mesh_vertices with a texture
    */
    
public:
	Cuboid(const float& width, const float& height, const float& depth);

	Cuboid() = delete;
	Cuboid(const Cuboid& other) = delete;
    void operator=(const Cuboid& other);

	virtual void render(const SHADER_ID& id);
	virtual void evaluate_changed();

	bool does_collide(const Collidable& other);
    
private:
	float width;
	float height;
	float depth;
};


