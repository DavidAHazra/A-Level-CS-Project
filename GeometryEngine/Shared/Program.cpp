#include "Program.hpp"


void Program::_link_program(const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geo) {
	const UInt vertex_id = _compile_shader(GL_VERTEX_SHADER, vertex_shader_path);
	const UInt fragment_id = _compile_shader(GL_FRAGMENT_SHADER, fragment_shader_path);

	if (geo != std::string()) {
		const UInt geo_id = _compile_shader(GL_GEOMETRY_SHADER, geo);
		_link_program_id(vertex_id, fragment_id, geo_id);
	} else { _link_program_id(vertex_id, fragment_id); }
}

void Program::_link_program_id(const UInt& vertex_id, const UInt& fragment_id, const long& geometry_id){
    const UInt program_id = glCreateProgram();
    
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    
    if (geometry_id > -1){
        glAttachShader(program_id, static_cast<unsigned int>(geometry_id));
    }
    
    glLinkProgram(program_id);
    
    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (success == 0) {
        char info_log[READ_BUFFER_SIZE];
        glGetProgramInfoLog(program_id, READ_BUFFER_SIZE, nullptr, info_log);
        throw std::runtime_error(std::string(info_log));
    }
    
    _program_id = program_id;
}

UInt Program::_compile_shader(const UInt& shader_type, const std::string& shader_path) {
	const UInt shader_id = glCreateShader(shader_type);

	if (!boost::filesystem::exists(shader_path)) {
		const std::string error_string = "Shader with name: '" + shader_path + "' does not exist";
		throw std::runtime_error(error_string.c_str());
	}

	std::ifstream file_stream(shader_path);
	const std::string file_string((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());
	const char* raw_source = file_string.c_str();

	glShaderSource(shader_id, 1, &raw_source, nullptr);
	glCompileShader(shader_id);

    check_compile_status(shader_id);

	return shader_id;
}

UInt Program::_compile_shader_string(const UInt& shader_type, const std::string& shader_string) {
	const UInt shader_id = glCreateShader(shader_type);

	const char* raw_source = shader_string.c_str();

	glShaderSource(shader_id, 1, &raw_source, nullptr);
	glCompileShader(shader_id);

    check_compile_status(shader_id);

	return shader_id;
}

void Program::check_compile_status(const UInt& id) {
    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        char info_log[READ_BUFFER_SIZE];
        glGetShaderInfoLog(id, READ_BUFFER_SIZE, nullptr, info_log);
        throw std::runtime_error(std::string(info_log));
    }
}
