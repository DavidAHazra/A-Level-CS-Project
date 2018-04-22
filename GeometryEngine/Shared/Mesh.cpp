#include "Mesh.hpp"
#include "ResourceHandler.hpp"


Mesh::Mesh(const std::vector<MeshVertex>& mesh_vertices, const std::vector<UInt>& indices, const std::vector<MeshTexture>& textures) :
    Shape(), mesh_vertices(mesh_vertices), indices(indices), textures(textures) {}

void Mesh::render(const SHADER_ID& id){
	SHADER_ID correct_id = id == SHADER_ID() ? GENERIC_ID() : id;
    
    Program& program = ResourceHandler::get_instance().get_program(correct_id);
    evaluate_changed();

    program.set_uniform<mat4>("model", get_model_matrix());

	if (correct_id == GENERIC_ID()) {
		// Bind appropriate textures
		for (size_t i = 0; i < textures.size(); ++i) {
			// Active proper texture unit before binding
			glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(i)); 
			glBindTexture(GL_TEXTURE_2D, textures.at(i).id);

			program.set_uniform<int>("material." + textures.at(i).type, static_cast<int>(i));
		}

		program.set_uniform<float>("material.shininess", 16.0f);
	}

    // Draw mesh
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
	if (correct_id == GENERIC_ID()) {
		for (size_t i = 0; i < textures.size(); ++i) {
			glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(i));
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

Mesh::Mesh(const Mesh& other) : Shape(), mesh_vertices(other.mesh_vertices), indices(other.indices), textures(other.textures) {
	quaternion = other.quaternion;
	rotation_point = other.rotation_point;
    translation_vector = other.translation_vector;
    move_vector = other.move_vector;
    enlargement = other.enlargement;
    
    _needs_evaluation = true;
}

void Mesh::operator=(const Mesh& other){
    this->mesh_vertices = other.mesh_vertices;
    this->indices = other.indices;
    this->textures = other.textures;
    
	quaternion = other.quaternion;
	rotation_point = other.rotation_point;
    translation_vector = other.translation_vector;
    move_vector = other.move_vector;
    enlargement = other.enlargement;
            
    _needs_evaluation = true;
}

void Mesh::evaluate_changed(){
    if (!_needs_evaluation) { return; }
    _needs_evaluation = false;
    
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &EBO);
    
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);
    
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh_vertices.size() * sizeof(MeshVertex), &mesh_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(UInt), &indices[0], GL_STATIC_DRAW);
    
    // Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), nullptr);
    
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, normal));
    
    // Vertex Textures
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, texture_coords));
    
    glBindVertexArray(0);
}
