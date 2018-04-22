#include "Shape.hpp"

void Shape::add_texture_to_resource_handler(const std::string& identifier, const UInt& id){
    ResourceHandler::get_instance().add_texture(identifier, id);
}

UInt Shape::load_texture_from_rgba(const float& red, const float& green, const float& blue, const float& alpha) {
    const Colour colour(red, green, blue, alpha);

    if (ResourceHandler::get_instance().does_contain_key(colour.to_string())){
        return ResourceHandler::get_instance().get_texture(colour.to_string());
    }
    
	UInt texture_id;
	glGenTextures(1, &texture_id);
	const float data[4] = { red, green, blue, alpha };

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
    
    Shape::add_texture_to_resource_handler(colour.to_string(), texture_id);

    return ResourceHandler::get_instance().get_texture(colour.to_string());
}

UInt Shape::load_texture_from_file(const std::string& path, const std::string& directory) {
	//Generate texture ID and load texture data
	const std::string file_name = (boost::filesystem::path(directory) / boost::filesystem::path(path)).string();
	if (!boost::filesystem::exists(boost::filesystem::path(file_name))) {
		throw std::runtime_error("File not found: " + file_name);
	}
    
    if (ResourceHandler::get_instance().does_contain_key(file_name.c_str())){
        return ResourceHandler::get_instance().get_texture(file_name.c_str());
    }

	UInt texture_id;
	glGenTextures(1, &texture_id);
    int width = 0;
    int height = 0;

	unsigned char* image = SOIL_load_image(file_name.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);

	const char* SOIL_load_result = SOIL_last_result();
	std::cout << "Loading Texture: " << path << " [Shape::load_texture_from_file] - [" << SOIL_load_result << "]" << std::endl;

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

    Shape::add_texture_to_resource_handler(file_name.c_str(), texture_id);
    return ResourceHandler::get_instance().get_texture(file_name.c_str());
}

vec3 Shape::get_vertex_position(const size_t& index){
    if (index > (vertices.size() / 8)) {
        throw std::runtime_error("Tried to get vertex that was not inside bounds");
    }
    
    const size_t first_position = index * 8;
    const vec4 position = (get_model_matrix() * vec4(vertices.at(first_position), 
		vertices.at(first_position + 1),
		vertices.at(first_position + 2), 
		1.0f));
    
    return to_vec3(position);
}

std::vector<vec3> Shape::get_personal_vertices() {
    std::vector<vec3> return_verts;
    
    for (size_t iter = 0; iter < vertices.size(); iter += 8){
        return_verts.push_back(vec3(vertices.at(iter), vertices.at(iter + 1), vertices.at(iter + 2)));
    }
    
    return return_verts;
}

