#pragma once

#include "Shape.hpp"

class Text : public Shape {
	struct FontCharacter {
		Vector2<int> character_size;
		Vector2<int> axial_bearing;
		UInt texture_id;
		UInt horisontal_char_offset;
	};

	const unsigned char ASCII_SIZE = 128;

public:
    inline static SHADER_ID GENERIC_ID() { return "Text"; };
    virtual inline SHADER_ID get_identifier(){ return Text::GENERIC_ID(); }
    
	Text(const std::string& font_path, const std::string& text = "Default Text");
    Text(const Text& other);
    void operator=(const Text& other) = delete;

    virtual void render(const SHADER_ID& id);
	virtual GLfloat get_height();
    virtual GLfloat get_width();
    
	inline void set_horisontal_align() { set_x(-get_width() / 2.0f); }
	inline void set_vertical_align() { set_y(-get_height() / 2.0f); }
	inline void set_to_center() { set_horisontal_align(); set_vertical_align(); }

	// Text
	inline std::string get_text() { return text; }
	inline void set_text(const std::string& new_text) { text = new_text; }
	
	// Font
	inline std::string get_font_path() { return font_path; }
	void set_font(const std::string& font_path);

	// Position
	inline GLfloat get_x() { return start_x; }
	inline void set_x(const GLfloat& new_x) { start_x = new_x; }

	inline GLfloat get_y() { return start_y; }
	inline void set_y(const GLfloat& new_y) { start_y = new_y; }

	// Size
	inline GLuint get_pixel_size() { return pixel_size; }
	void set_pixel_size(const GLuint& new_pixel_size);

	// Scale
	inline GLfloat get_scale() { return scale; }
	inline void set_scale(const GLfloat& new_scale) { scale = new_scale; }
    
    // Colour
    inline Colour get_colour() { return colour; }
    inline void set_colour(const Colour& colour) { this->colour = colour; }

protected:
	virtual void evaluate_changed();

	std::string text;
	std::string font_path;

	float start_x = 0.0f;
	float start_y = 0.0f;
    
	UInt pixel_size = 200;
	float scale = 1.0f;

	void _create_chars();
	std::map<char, FontCharacter> chars;
    
    Colour colour = Colours::DEBUG_COLOUR;
};

