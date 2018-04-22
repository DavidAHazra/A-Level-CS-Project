#include "Rectangle.hpp"
#include "ResourceHandler.hpp"

Rect::Rect(const float& width, const float& height) : Shape(), width(width), height(height) {
    const float half_width = width / 2.0f;
    const float half_height = height / 2.0f;
    vertices = {
        // This variable is local to this function because (as opposed to other shapes) its dimensions can vary
        
        -half_width, -half_height, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        half_width, half_height, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -half_width, half_height, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        half_width, half_height, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -half_width, -half_height, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        half_width, -half_height, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
    };
    
    texture = Shape::load_texture_from_rgba(Colours::WHITE);
}

void Rect::render(const SHADER_ID& id) {
	const SHADER_ID to_use = id == SHADER_ID() ? get_identifier() : id;
    ResourceHandler& instance = ResourceHandler::get_instance();
    Program& program = instance.get_program(to_use);

	evaluate_changed();

    program.set_uniform<mat4>("model", get_model_matrix());
	program.set_uniform<float>("material.shininess", 16.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	program.set_uniform<int>("material.texture_diffuse", 0);
	program.set_uniform<int>("material.texture_specular", 0);
    
		glBindVertexArray(_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
    
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Rect::evaluate_changed() {
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
