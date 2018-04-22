#pragma once
#include "Shape.hpp"


class Rect : public Shape {
public:
    Rect(const Rect& other) = delete;
    void operator=(const Rect& other) = delete;

	Rect(const float& width, const float& height);

    virtual inline ~Rect() {}
    virtual void render(const SHADER_ID& id);
    virtual inline void set_texture(const UInt& new_tex) { texture = new_tex; }
            
protected:
	virtual void evaluate_changed();

	UInt texture;
    
    float width;
    float height;
};

