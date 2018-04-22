#pragma once

#include "EngineHeader.hpp"


// Template Specialisation
namespace Uniforms {
    template <typename T>
    static inline void set_uniform(const UInt&, const std::string&, const T&, bool _ = true) {
        throw std::runtime_error("Tried to set a uniform when the type was not valid");
    }
    
    template <>
    inline void set_uniform<float>(const UInt& id, const std::string& uniform_name, const float& value, bool use_program) {
        if (use_program) { glUseProgram(id); }
        glUniform1f(glGetUniformLocation(id, uniform_name.c_str()), value);
    }
    
    template <>
    inline void set_uniform<int>(const UInt& id, const std::string& uniform_name, const int& value, bool use_program) {
        if (use_program) { glUseProgram(id); }
        glUniform1i(glGetUniformLocation(id, uniform_name.c_str()), value);
    }
    
    template<>
    inline void set_uniform<bool>(const UInt& id, const std::string& uniform_name, const bool& value, bool use_program) {
        set_uniform<int>(id, uniform_name, value ? 1 : 0, use_program);
    }
    
    template <>
    inline void set_uniform<vec3>(const UInt& id, const std::string& uniform_name, const vec3& value, bool use_program) {
        if (use_program) { glUseProgram(id); }
        glUniform3f(glGetUniformLocation(id, uniform_name.c_str()), value.x, value.y, value.z);
    }
    
    template <>
    inline void set_uniform<vec4>(const UInt& id, const std::string& uniform_name, const vec4& value, bool use_program) {
        if (use_program) { glUseProgram(id); }
        glUniform4f(glGetUniformLocation(id, uniform_name.c_str()), value.x, value.y, value.z, value.w);
    }
    
    template <>
    inline void set_uniform<mat4>(const UInt& id, const std::string& uniform_name, const mat4& value, bool use_program) {
        if (use_program) { glUseProgram(id); }
        glUniformMatrix4fv(glGetUniformLocation(id, uniform_name.c_str()), 1, GL_FALSE, value_ptr(value));
    }
}


class Program {
    static const UInt READ_BUFFER_SIZE = 4096;

public:
	Program(const Program& other) = delete;
	void operator=(const Program& other) = delete;

public:
    inline Program(const std::string& vert, const std::string& frag, const std::string& geo = std::string()) { _link_program(vert, frag, geo); }
	virtual ~Program() { glDeleteProgram(_program_id); }

	inline UInt get_program_id() const { return _program_id; }
	inline void use() const { glUseProgram(_program_id); }
    inline void stop_using() const { glUseProgram(0); }
    
    template <typename T>
    inline void set_uniform(const std::string& uniform_name, const T& value, const bool use_program = true){
        Uniforms::set_uniform<T>(_program_id, uniform_name, value, use_program);
    }

private:
	void _link_program(const std::string& vertex, const std::string& fragment, const std::string& geo = std::string());
    
    static UInt _compile_shader(const UInt& shader_type, const std::string& shader_name);
    static void check_compile_status(const UInt& id);
    
protected:
	inline Program() {}
    UInt _program_id;
    
    void _link_program_id(const UInt& vertex_id, const UInt& fragment_id, const long& geometry_id = -1);
	static UInt _compile_shader_string(const UInt& shader_type, const std::string& shader_string);
};

