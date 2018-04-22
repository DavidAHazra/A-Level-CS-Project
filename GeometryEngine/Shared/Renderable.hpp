#pragma once
#include "EngineHeader.hpp"

class Renderable {
public:
    Renderable(const Renderable& other) = delete;
    void operator=(const Renderable& other) = delete;

	inline Renderable() {}
	virtual ~Renderable();
    virtual void render(const SHADER_ID& id = SHADER_ID()) = 0;
    virtual SHADER_ID get_identifier() = 0; // Used so that ResourceHandler knows which shaders to use for derived
    
protected:
	virtual void evaluate_changed() = 0;

	bool _needs_evaluation = true;
    
	UInt _VAO = 0;
	UInt _VBO = 0;
};

