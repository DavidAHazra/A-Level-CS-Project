#include "SkyBox.hpp"
#include "ResourceHandler.hpp"

SkyBox::SkyBox(const std::string& enclosing_dir_path) : Cube() {
    const std::vector<std::string> wanted_files = {
        "Right.png", "Left.png", "Top.png", "Bottom.png", "Front.png", "Back.png"
    };
    
    boost::filesystem::path dir_path(enclosing_dir_path);
    
    if (!(boost::filesystem::exists(dir_path) && boost::filesystem::is_directory(dir_path))){
        throw std::runtime_error("SkyBox Path: " + enclosing_dir_path + " does not exist or is not a directory");
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
    int width, height;
    unsigned char* image;
    for (size_t i = 0; i < wanted_files.size(); ++i){
        boost::filesystem::path comb = (dir_path / wanted_files.at(i)).make_preferred();
        
        if (!boost::filesystem::exists(comb)){
            throw std::runtime_error("SkyBox File: " + comb.string() + " does not exist");
        }
        
        image = SOIL_load_image(comb.string().c_str(), &width, &height, nullptr, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<unsigned int>(i), 0, GL_RGB,
                     width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		SOIL_free_image_data(image);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

SkyBox::~SkyBox() {

}


void SkyBox::render(const SHADER_ID& id){
    SHADER_ID to_use = id == SHADER_ID() ? get_identifier() : id;
    ResourceHandler& instance = ResourceHandler::get_instance();
    Program& program = instance.get_program(to_use);
    
    evaluate_changed();
    
    program.set_uniform<mat4>("model", get_model_matrix());
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
    program.set_uniform<int>("texture_cube", 0);
    
    glCullFace(GL_FRONT);
    
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    glCullFace(GL_BACK);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void SkyBox::evaluate_changed(){
    if (!_needs_evaluation) { return; }
    _needs_evaluation = false;
    
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);
    
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->vertices.size(), &this->vertices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
