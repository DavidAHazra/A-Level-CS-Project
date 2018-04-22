#pragma once
#include "Cube.hpp"


class SkyBox : public Cube {
    /*
     To use, put 6 images in a folder (enclosing_dir_path) and name them:
     
        Left.png
        Right.png
        Top.png
        Bottom.png
        Front.png
        Back.png
     */
    
public:
    SkyBox(const std::string& enclosing_dir_path);
	virtual ~SkyBox();
    
    SkyBox() = delete;
    SkyBox(const SkyBox& other) = delete;
    void operator=(const SkyBox& other) = delete;
    
	inline static SHADER_ID GENERIC_ID() { return "SkyBox"; };
	inline virtual SHADER_ID get_identifier() { return SkyBox::GENERIC_ID(); }
        
    virtual void render(const SHADER_ID& id);
    
protected:
    virtual void evaluate_changed();
};

