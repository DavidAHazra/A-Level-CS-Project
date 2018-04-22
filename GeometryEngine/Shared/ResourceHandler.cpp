#include "ResourceHandler.hpp"


ResourceHandler::~ResourceHandler(){
    for (auto iter = _programs.begin(); iter != _programs.end(); iter++){
        delete iter->second;
		iter->second = nullptr;
    }
    
    _programs.clear();
    
    for (std::map<std::string, UInt>::iterator iter = _textures.begin(); iter != _textures.end(); iter++){
        glDeleteTextures(1, &iter->second);
    }
    
    _textures.clear();
}
