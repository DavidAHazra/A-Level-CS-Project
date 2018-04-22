#include "LightMapProgram.hpp"


LightMapProgram::LightMapProgram(const std::string& vertex_shader_path,
                                 const std::string& fragment_shader_path,
                                 const std::string& geometry_shader_path) :
vertex_path(vertex_shader_path),
fragment_path(fragment_shader_path),
geometry_path(geometry_shader_path) {
    
	set_light_count();
}

LightMapProgram::~LightMapProgram() {
	reset();
}

void LightMapProgram::reset() {
	MemoryManagement::delete_all_from_vector(lights);
}

void LightMapProgram::add_light(PointLight* light) {
	lights.push_back(light);
	set_light_count();

    for (size_t iter = 0; iter < get_light_count(); ++iter){
        std::string light_index = std::to_string(iter);
        PointLight* light = lights.at(iter);
        
        set_uniform<vec3>("lights[" + light_index + "].position", light->position);
        set_uniform<vec3>("lights[" + light_index + "].light_colour", light->light_colour);
        
        set_uniform<float>("lights[" + light_index + "].constant_val", light->constant_val);
        set_uniform<float>("lights[" + light_index + "].linear_val", light->linear_val);
        set_uniform<float>("lights[" + light_index + "].quadratic", light->quadratic);
        
        set_uniform<vec3>("lights[" + light_index + "].ambient", light->ambient);
        set_uniform<vec3>("lights[" + light_index + "].diffuse", light->diffuse);
        set_uniform<vec3>("lights[" + light_index + "].specular", light->specular);
    }
}

void LightMapProgram::set_light_position(const UInt& light_index, const vec3& new_pos) {
	set_uniform<vec3>("lights[" + std::to_string(light_index) + "].position", new_pos);
	lights.at(light_index)->position = new_pos;
}

void LightMapProgram::set_light_colour(const UInt& light_index, const vec3& colour){
    set_uniform<vec3>("lights[" + std::to_string(light_index) + "].light_colour", colour);
    lights.at(light_index)->light_colour = colour;
}

void LightMapProgram::set_light_count(){
    glDeleteProgram(_program_id);
        
    if (!boost::filesystem::exists(vertex_path)) { throw std::runtime_error(vertex_path + " does not exist"); }
    else if (!boost::filesystem::exists(fragment_path)) { throw std::runtime_error(fragment_path + " does not exist"); }
    
    std::ifstream vertex_file_stream(vertex_path);
    const std::string vertex_file_string((std::istreambuf_iterator<char>(vertex_file_stream)), std::istreambuf_iterator<char>());
    
    std::ifstream fragment_file_stream(fragment_path);
    const std::string fragment_file_string((std::istreambuf_iterator<char>(fragment_file_stream)), std::istreambuf_iterator<char>());
    std::string modified_fragment_string;
    
    std::string geometry_file_string;
    if (geometry_path != std::string()){
        std::ifstream geometry_file_stream(geometry_path);
        geometry_file_string = std::string((std::istreambuf_iterator<char>(geometry_file_stream)), std::istreambuf_iterator<char>());
    }
    
    for (size_t index = 0; index < fragment_file_string.size(); index++){
        const char current_char = fragment_file_string.at(index);
        modified_fragment_string.append(current_char == '$' ? std::to_string(get_light_count()) : std::string(1, current_char));
    }
    
    const UInt vertex_id = _compile_shader_string(GL_VERTEX_SHADER, vertex_file_string);
    const UInt fragment_id = _compile_shader_string(GL_FRAGMENT_SHADER, modified_fragment_string);

    if (geometry_path != std::string()){
        UInt geometry_id = _compile_shader_string(GL_GEOMETRY_SHADER, geometry_file_string);
        _link_program_id(vertex_id, fragment_id, geometry_id);
    } else { _link_program_id(vertex_id, fragment_id); }
}
