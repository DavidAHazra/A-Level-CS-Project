#include "Cuboid.hpp"
#include "ResourceHandler.hpp"

Cuboid::Cuboid(const float& width, const float& height, const float& depth) : Cube(), width(width), height(height), depth(depth) {
	// Using short names for aesthetics when defining all mesh_vertices
	// Using half the width, height and depth because I want the "position" to be at the centre
	const float w = width / 2.0f;
	const float h = height / 2.0f;
	const float d = depth / 2.0f;

	vertices = {
		// Back face
		-w, -h, -d, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// bottom-left
		w, h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
		w, -h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,	// bottom-right
		w, h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
		-w, -h, -d, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// bottom-left
		-w, h, -d, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,	// top-left

		// Front face
		-w, -h, d, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// bottom-left
		w, -h, d, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,		// bottom-right
		w, h, d, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,		// top-right
		w, h, d, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,		// top-right
		-w, h, d, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,		// top-left
		-w, -h, d, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// bottom-left

		// Left face
		-w, h, d, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
		-w, h, -d, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-left
		-w, -h, -d, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-left
		-w, -h, -d, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-left
		-w, -h, d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
		-w, h, d, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right

		// Right face
		w, h, d, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
		w, -h, -d, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-right
		w, h, -d, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,		// top-right         
		w, -h, -d, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-right
		w, h, d, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
		w, -h, d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,		// bottom-left

		// Bottom face
		-w, -h, -d, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,	// top-right
		w, -h, -d, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,	// top-left
		w, -h, d, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
		w, -h, d, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
		-w, -h, d, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
		-w, -h, -d, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,	// top-right

		// Top face
		-w, h, -d, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// top-left
		w, h, d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,		// bottom-right
		w, h, -d, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,		// top-right     
		w, h, d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,		// bottom-right
		-w, h, -d, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// top-left
		-w, h, d, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f		// bottom-left   
	};
}

void Cuboid::render(const SHADER_ID& id) {
	SHADER_ID to_use = id == SHADER_ID() ? get_identifier() : id;
	ResourceHandler& instance = ResourceHandler::get_instance();
	Program& program = instance.get_program(to_use);

	evaluate_changed();

	program.set_uniform<mat4>("model", get_model_matrix());
	program.set_uniform<float>("material.shininess", 16);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	program.set_uniform<int>("material.texture_diffuse", 0);
	program.set_uniform<int>("material.texture_specular", 0);

	glBindVertexArray(_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void Cuboid::operator=(const Cuboid& other){
    height = other.height;
    width = other.width;
    depth = other.depth;
    vertices = other.vertices;
}

void Cuboid::evaluate_changed() {
	if (!_needs_evaluation) { return; }
	_needs_evaluation = false;

	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);

	glGenVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);

	glGenBuffers(1, &_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool Cuboid::does_collide(const Collidable& other) {
	Collidable this_collidable = Collidable::make_collidable(*this);
	return this_collidable.bounding_box.does_collide(this_collidable.model_matrix, this_collidable.local_axis, other.bounding_box, other.model_matrix, other.local_axis);
}
