#pragma once

#include "IncludeOrder.hpp"
#include "Maths.hpp"

// Shorter type definitions for convenience
typedef unsigned short UShort;
typedef unsigned int UInt;
typedef int EnumType;
typedef std::string SHADER_ID;


namespace EngineConstants {
    static const mat4 IDENTITY_MATRIX = mat4(1.0f);
}


namespace Random {
    static std::mt19937 generator(static_cast<unsigned int>(time(nullptr)));
    
    template <typename T>
    static T get_random(const T& start, const T& end) {
        throw std::runtime_error("Invalid template argument for get_random<T>");
    }
    
    template<>
    inline int get_random<int>(const int& start, const int& end) {
        return std::uniform_int_distribution<int>(start, end)(generator);
    }
    
    template<>
    inline float get_random<float>(const float& start, const float& end) {
        return std::uniform_real_distribution<float>(start, end)(generator);
    }
}


namespace MemoryManagement {
    template <typename Type, typename Allocator>
    static void delete_all_from_vector(std::vector<Type, Allocator>& vector) {
        for (size_t iter = 0; iter < vector.size(); ++iter) {
            delete vector[iter];
            vector[iter] = nullptr;
        }
        
        vector.erase(vector.begin(), vector.end());
    }
}


namespace Maths {
    static quat rotate_between_vectors(vec3 start, vec3 end) {
        start = normalise(start);
        end = normalise(end);
        
        const float cos_theta = dot(start, end);
        vec3 rotation_axis;
        
        if (cos_theta < (-1.0f + Constants::EPSILON)) {
            rotation_axis = cross(vec3(0.0f, 0.0f, 1.0f), start);
            if (length_squared(rotation_axis) < 0.01f) {
                // Vectors were parallel, unable to rotate between them easily
                rotation_axis = cross(vec3(1.0f, 0.0f, 0.0f), start);
            }
            
            rotation_axis = normalise(rotation_axis);
            return quat_from_angle_axis(static_cast<float>(M_PI), rotation_axis);
        }
        
        rotation_axis = cross(start, end);
        
        const float s = std::sqrt((1.0f + cos_theta) * 2.0f);
        const float invs = 1.0f / s;
        
        return quat(s / 2.0f,
                         rotation_axis.x * invs,
                         rotation_axis.y * invs,
                         rotation_axis.z * invs
                         );
    }
    
    static quat look_at(vec3 direction, vec3 desired_up) {
        if (length_squared(direction) < Constants::EPSILON_2) { return quat(); }
        
        const vec3 right = cross(direction, desired_up);
        desired_up = cross(right, direction);
        
        const quat rotation_1 = rotate_between_vectors(vec3(0.0f, 0.0f, 1.0f), direction);
        const vec3 new_up = rotation_1 * vec3(0.0f, 1.0f, 0.0f);
        const quat rotation_2 = rotate_between_vectors(new_up, desired_up);
        
        return rotation_2 * rotation_1; // Quaternion multiplication in reverse
    }   
}


namespace ColourNamespace {
    struct Colour {
        Colour(const float& whiteness)
        : red(whiteness), green(whiteness), blue(whiteness), alpha(1.0f) {}
        
        Colour(const float& red, const float& green, const float& blue, const float& alpha = 1.0f)
        : red(red), green(green), blue(blue), alpha(alpha) {}
        
        inline bool operator==(const Colour& other) const {
            return (red == other.red) && (green == other.green) && (blue == other.blue) && (alpha == other.alpha);
        }
        
        inline std::string to_string() const {
            return std::to_string(red) + std::to_string(green) + std::to_string(blue) + std::to_string(alpha);
        }
        
        // RGBA
        float red;
        float green;
        float blue;
        float alpha;
    };
    
    namespace Colours {
        // RED GREEN BLUE (ALPHA) (RGB(A))
        static const Colour BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };
        static const Colour WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
        
        static const Colour RED = { 1.0f, 0.0f, 0.0f, 1.0f };
        static const Colour GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
        static const Colour BLUE = { 0.0f, 0.0f, 1.0f, 1.0f };
        
        static const Colour PINK = { 1.0f, 0.0f, 1.0f, 1.0f };
        static const Colour YELLOW = { 1.0f, 1.0f, 0.0f, 1.0f };
        
        static const Colour PURPLE = { 0.58f, 0.0f, 0.83f, 1.0f };
        static const Colour CRIMSON = { 0.86f, 0.078f, 0.24f, 1.0f };
        static const Colour ORANGE = { 0.8f, 0.44f, 0.0f, 1.0f };
        
        static const Colour DEBUG_COLOUR = PINK;
    }
}


static void check_errors(const std::string& extra = std::string()) {
    UInt opengl_error = glGetError();
    if (opengl_error != GL_NO_ERROR) {
        throw std::runtime_error(std::to_string(opengl_error) + (extra == std::string() ? "" : " [At: " + extra + "]"));
    }
}

static const std::map<int, char> GLFW_key_map = {
    // Letters
    { GLFW_KEY_A, 'A' }, { GLFW_KEY_B, 'B' }, { GLFW_KEY_C, 'C' },
    { GLFW_KEY_D, 'D' }, { GLFW_KEY_E, 'E' }, { GLFW_KEY_F, 'F' },
    { GLFW_KEY_G, 'G' }, { GLFW_KEY_H, 'H' }, { GLFW_KEY_I, 'I' },
    { GLFW_KEY_J, 'J' }, { GLFW_KEY_K, 'K' }, { GLFW_KEY_L, 'L' },
    { GLFW_KEY_M, 'M' }, { GLFW_KEY_N, 'N' }, { GLFW_KEY_O, 'O' },
    { GLFW_KEY_P, 'P' }, { GLFW_KEY_Q, 'Q' }, { GLFW_KEY_R, 'R' },
    { GLFW_KEY_S, 'S' }, { GLFW_KEY_T, 'T' }, { GLFW_KEY_U, 'U' },
    { GLFW_KEY_V, 'V' }, { GLFW_KEY_W, 'W' }, { GLFW_KEY_X, 'X' },
    { GLFW_KEY_Y, 'Y' }, { GLFW_KEY_Z, 'Z' },
    
    // Numbers
    { GLFW_KEY_1, '1' }, { GLFW_KEY_2, '2' }, { GLFW_KEY_3, '3' },
    { GLFW_KEY_4, '4' }, { GLFW_KEY_5, '5' }, { GLFW_KEY_6, '6' },
    { GLFW_KEY_7, '7' }, { GLFW_KEY_8, '8' }, { GLFW_KEY_9, '9' },
    { GLFW_KEY_0, '0' },
    
    // Special Characters
    { GLFW_KEY_MINUS, '-' },
    { GLFW_KEY_EQUAL, '=' },
    { GLFW_KEY_SEMICOLON, ';' },
    { GLFW_KEY_APOSTROPHE, '\'' },
    { GLFW_KEY_SLASH, '\\' },
    { GLFW_KEY_COMMA, ',' },
    { GLFW_KEY_PERIOD, '.' },
    { GLFW_KEY_BACKSLASH, '/' }
};


// Namespace used to compartmentalise file only, not used to stop potential global namespace pollution
using namespace Random;
using namespace ColourNamespace;
using namespace Maths;
