#include "Renderable.hpp"

Renderable::~Renderable(){
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
}
