#pragma once
#include "EngineHeader.hpp"

#ifdef IS_WINDOWS
	class EXE_PATH {	// Class used ONLY to store the path of the exe on windows for filesystem operations
	public:
		EXE_PATH(const EXE_PATH& other) = delete;
		void operator=(const EXE_PATH& other) = delete;

		static EXE_PATH& get_instance() {
			static EXE_PATH instance;
			return instance;
		}

		inline boost::filesystem::path get() { return _path; }
		inline void set(const boost::filesystem::path& path) { _path = path; }

	private:
		EXE_PATH() {}
		boost::filesystem::path _path;
	};
#endif

template <typename T>
class Attribute {
public:
    inline Attribute(const T& attribute) : attribute(attribute) {}
    inline Attribute(const Attribute& other) : attribute(other.attribute) {}
    inline void operator=(const Attribute& other) { this->attribute = other.attribute; }
    
    inline void set_attribute(const T& new_attribute) { attribute = new_attribute; }
    inline T get_attribute() const { return attribute; }
    
private:
    T attribute;
};

namespace FileSystem {
	namespace fs = boost::filesystem;

	static inline fs::path join(const fs::path& path1, const fs::path& path2) { return (path1 / path2).make_preferred(); }
	static inline fs::path get_resource_dir() {
#ifdef IS_APPLE
        return join(OSXHelpers::get_resource_path(), "Resources");
#else
		// TODO: When distributing, uncomment bottom line
        //return join(join(EXE_PATH::get_instance().get().parent_path().parent_path().parent_path(), "Shared"), "Resources");
		return join(join(EXE_PATH::get_instance().get().parent_path(), "Shared"), "Resources");
#endif
    }
    
	static inline fs::path get_shaders_dir() { return join(get_resource_dir(), "shaders"); }
	static inline fs::path get_fonts_dir() { return join(get_resource_dir(), "fonts"); }
    static inline fs::path get_meshes_dir() { return join(get_resource_dir(), "meshes"); }
	static inline fs::path get_textures_dir() { return join(get_resource_dir(), "textures"); }
    static inline fs::path get_shader(const std::string& name){ return join(get_shaders_dir(), name); }
    static inline fs::path get_font(const std::string& name){ return join(get_fonts_dir(), name); }
    static inline fs::path get_mesh(const std::string& name) { return join(get_meshes_dir(), name); }
	static inline fs::path get_texture(const std::string& name) { return join(get_textures_dir(), name); }
}

namespace GameConstants {
	static const std::string GAME_NAME = "Survive.";

	// This is a function because the value is determined at runtime, dispite being constant
	static inline std::string MECHA() { return FileSystem::get_font("Mecha.ttf").string(); }
	static inline std::string ARIAL() { return FileSystem::get_font("Arial.ttf").string(); }
	static inline std::string GOMARICE() { return FileSystem::get_font("Gomarice.ttf").string(); }
    
    static inline std::string OPTIONS() { return FileSystem::join(FileSystem::get_resource_dir(), "OPTIONS").string(); }
    static inline std::string LEADERBOARD() { return FileSystem::join(FileSystem::get_resource_dir(), "LEADERBOARD").string(); }

#ifdef IS_WINDOWS
    static const int framebuffer_depth_resolution = 1024;
#else
    static const int framebuffer_depth_resolution = 512;
#endif
    
    static const float far_plane = 5000.0f;
    static const float near_plane = 0.1f;
    static const float FOV = 1.0471975512f; // 60 degrees in radians (pi / 3)
    static const float SHADOW_FOV = 1.57079632679f; // 90 degrees in radians (pi / 2)

	static const mat4 shadow_proj = perspective(GameConstants::SHADOW_FOV, 1.0f, GameConstants::near_plane, GameConstants::far_plane);
}

namespace Options {
	static const std::string MULTISAMPLING = "MULTISAMPLING";
	static const std::string SHADOWS = "SHADOWS";
	static const std::string FULLSCREEN = "FULLSCREEN";
}

template <typename First, typename Second>
struct GreaterSecondPairSort {
    typedef std::pair<First, Second> Pair;
    
    bool operator()(const Pair& left, const Pair& right) {
        return left.second > right.second;
    }
};










