#pragma once

#include "LightMapProgram.hpp"
#include "Shape.hpp"


class ResourceHandler {
public:
	static ResourceHandler& get_instance() {
		static ResourceHandler instance;
		return instance;
	}
    
    // Shaders
	inline void load_program(const std::string& vertex, const std::string& fragment, const SHADER_ID& program_id) {
        _programs.insert(std::pair<SHADER_ID, Program*>(program_id, new Program(vertex, fragment)));
	}

	inline void load_program(const std::string& vertex, const std::string& fragment, const std::string& geo, const SHADER_ID& id) {
		_programs.insert(std::pair<SHADER_ID, Program*>(id, new Program(vertex, fragment, geo)));
	}

	inline void load_lightmap(const std::string& vertex, const std::string& fragment, const SHADER_ID& id) {
        _programs.insert({ id, new LightMapProgram(vertex, fragment) });
	}

	inline void load_lightmap(const std::string& vertex, const std::string& fragment, const std::string geo, const SHADER_ID& id) {
        _programs.insert({ id, new LightMapProgram(vertex, fragment, geo) });
	}

	inline Program& get_program(const SHADER_ID& program_id) { return *_programs.at(program_id); }
    
    // Textures
    inline void add_texture(const std::string& id, const UInt& texture_id){
        _textures.insert({ id, texture_id });
    }
    
    inline bool does_contain_key(const std::string& key){
        return _textures.find(key) != _textures.end();
    }
        
    inline UInt get_texture(const std::string& texture_id) { return _textures.at(texture_id); }

	ResourceHandler(const ResourceHandler& other) = delete;
	void operator=(const ResourceHandler& other) = delete;

private:
	inline ResourceHandler() {}
    ~ResourceHandler();
	std::map<SHADER_ID, Program*> _programs;
    std::map<std::string, UInt> _textures;
};

