#pragma once
#include "Shape.hpp"



class Cube : public Shape {
public:
	Cube();
    Cube(const Cube& other) = delete;
    void operator=(const Cube& other) = delete;

	virtual ~Cube();

    virtual void render(const SHADER_ID& id);

    virtual inline void set_texture(const UInt& new_tex) { texture = new_tex; }
            
protected:
	virtual void evaluate_changed();

	UInt texture;
};

