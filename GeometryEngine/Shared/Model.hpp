#pragma once

#include "Mesh.hpp"

class Model : public Shape {
    // *Basically* a collection of meshes
    
public:
    Model(const std::string& model_path);
    Model(const Model& other) = delete;
    void operator=(const Model& other) = delete;

	virtual ~Model();
    
    virtual void render(const SHADER_ID& id);
    
    std::vector<Mesh> meshes;

	virtual std::vector<vec3> get_personal_vertices();
    
protected:
    virtual void evaluate_changed();
    
private:
    std::string model_file_path;
    std::string model_directory;

	// This container is static so that if other models use the exact same textures, they don't need to be loaded more than once
    static std::vector<MeshTexture> loaded_textures;
	static int model_count;
    
    void load();
    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
    void load_texture(std::vector<MeshTexture>& load_vector, aiMaterial* material, aiTextureType type, const std::string& type_name);
};

 
