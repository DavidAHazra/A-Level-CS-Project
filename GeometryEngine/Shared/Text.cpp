#include "Text.hpp"
#include "ResourceHandler.hpp"


Text::Text(const std::string& font_path, const std::string& text) : Shape(), text(text), font_path(font_path){
	_create_chars();
}

Text::Text(const Text& other) : Shape(), text(other.text), font_path(other.font_path) {
	_needs_evaluation = true;
	quaternion = other.quaternion;
	rotation_point = other.rotation_point;
	translation_vector = other.translation_vector;
	move_vector = other.move_vector;
	scale = other.scale;
	enlargement = other.enlargement;
	start_x = other.start_x;
	start_y = other.start_y;
	pixel_size = other.pixel_size;

	_create_chars();
}

void Text::set_font(const std::string& new_font) {
	if (!boost::filesystem::exists(new_font)) {
		std::string error_string = "Font: '" + new_font + "' does not exist";
		throw std::runtime_error(error_string.c_str());
	}

	font_path = new_font;
	_create_chars();
}

void Text::set_pixel_size(const UInt& new_pixel_size) {
	pixel_size = new_pixel_size;
	_create_chars();
}

void Text::_create_chars() {
	chars.clear();

	FT_Library freetype_library;
	if (FT_Init_FreeType(&freetype_library)) {
		throw std::runtime_error("Failed to initialise FreeType library");
	}

	FT_Face type_face;
	if (FT_New_Face(freetype_library, font_path.c_str(), 0, &type_face)) {
		throw std::runtime_error("FreeType failed to load font, text: " + text + '\n' + "Font: " + font_path);
	}

	FT_Set_Pixel_Sizes(type_face, 0, pixel_size);

	for (unsigned char char_value = 0; char_value < ASCII_SIZE; char_value++){
		if (FT_Load_Char(type_face, char_value, FT_LOAD_RENDER)){
			continue;
		}

		UInt texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, type_face->glyph->bitmap.width, type_face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, type_face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		FontCharacter character = {
			Vector2<int>(type_face->glyph->bitmap.width, type_face->glyph->bitmap.rows),
			Vector2<int>(type_face->glyph->bitmap_left, type_face->glyph->bitmap_top),
			texture,
			static_cast<UInt>(type_face->glyph->advance.x)
		};

		chars.insert(std::pair<GLchar, FontCharacter>(char_value, character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(type_face);
	FT_Done_FreeType(freetype_library);
}

void Text::render(const SHADER_ID& id) {
	evaluate_changed();
    
    ResourceHandler& instance = ResourceHandler::get_instance();
	SHADER_ID to_use = id == SHADER_ID() ? get_identifier() : id;
    Program& text_program = instance.get_program(to_use);

	text_program.set_uniform<mat4>("model", get_model_matrix());
    text_program.set_uniform<vec3>("text_colour", vec3(colour.red, colour.green, colour.blue));

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(_VAO);

	GLfloat x = start_x;
	GLfloat y = start_y;
    
	for (size_t character_index = 0; character_index < text.size(); character_index++){
		FontCharacter character = chars.at(text.at(character_index));

		float x_pos = x + character.axial_bearing.x * scale;
		float y_pos = y - (character.character_size.y - character.axial_bearing.y) * scale;

		float width = character.character_size.x * scale;
		float height = character.character_size.y * scale;

		float vertices[6][4] = {  // Raw array because mesh_vertices aren't stored
			{ x_pos,		 y_pos + height,  0.0f, 0.0f },
			{ x_pos,		 y_pos,			0.0f, 1.0f },
			{ x_pos + width, y_pos,			1.0f, 1.0f },

			{ x_pos,		 y_pos + height,  0.0f, 0.0f },
			{ x_pos + width, y_pos,			1.0f, 1.0f },
			{ x_pos + width, y_pos + height,  1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, character.texture_id);

		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        
        text_program.set_uniform<int>("text", 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
        GLfloat offset = (character.horisontal_char_offset >> 6) * scale;
		x += offset;
    }
    
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::evaluate_changed() {
	if (!_needs_evaluation) { return; }
	_needs_evaluation = false;

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glBindVertexArray(_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLfloat Text::get_height() {
    GLfloat max_height = chars.at(text.at(0)).character_size.y * scale;
    for (size_t character_index = 0; character_index < text.size(); character_index++) {
        FontCharacter character = chars.at(text.at(character_index));
        GLfloat height = character.character_size.y * scale;
        max_height = (height > max_height) ? height : max_height;
    }
    
    return max_height;
}

GLfloat Text::get_width(){
    GLfloat width = 0.0f;
    for (size_t character_index = 0; character_index < text.size(); character_index++){
        FontCharacter character = chars.at(text.at(character_index));
        
        width += ((character.horisontal_char_offset >> 6) * scale);
    }

    return width;
}
