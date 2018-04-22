#include "Model.hpp"

std::vector<MeshTexture> Model::loaded_textures = {};
int Model::model_count = 0;

Model::Model(const std::string& model_path) : Shape(), model_file_path(model_path) {
    if (!boost::filesystem::exists(model_path)){ throw std::runtime_error("Model at: " + model_path + " does not exist"); }
    
    load();
	Model::model_count++;
}

Model::~Model() {
	Model::model_count--;
}

void Model::load(){
    Assimp::Importer importer;
	int flags =
		aiProcess_Triangulate				|
		aiProcess_FlipUVs					|
		aiProcess_GenNormals				|
		aiProcess_OptimizeMeshes			|
		aiProcess_OptimizeGraph				|
		aiProcess_FindInstances				|
		aiProcess_FindInvalidData			|
		aiProcess_FindDegenerates			|
		aiProcess_FixInfacingNormals		|
		aiProcess_RemoveRedundantMaterials	|
		aiProcess_ImproveCacheLocality		|
        aiProcess_JoinIdenticalVertices     |
		aiProcess_ValidateDataStructure;

	// aiProcess_GenSmoothNormals
    const aiScene* scene = importer.ReadFile(model_file_path, flags);
    
    if ((!scene) || (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode)){
        throw std::runtime_error(std::string("Assimp error: ") + importer.GetErrorString());
    }

    model_directory = boost::filesystem::path(model_file_path).parent_path().string();
    
    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene){
    // Using recursion instead of iteration because this defines a unique parent-child structure
    
    for (size_t mesh_iter = 0; mesh_iter < node->mNumMeshes; ++mesh_iter){
        // Add mesh
        meshes.push_back(process_mesh(scene->mMeshes[node->mMeshes[mesh_iter]], scene));
    }
    
    for (size_t child_iter = 0; child_iter < node->mNumChildren; ++child_iter){
        // Recur process for each child
        process_node(node->mChildren[child_iter], scene);
    }
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene){
    std::vector<MeshVertex> mesh_vertices;
    std::vector<UInt> vertex_indices;
    std::vector<MeshTexture> textures;

	for (size_t mesh_iter = 0; mesh_iter < mesh->mNumVertices; ++mesh_iter){
        MeshVertex vertex;
        
        vertex.position = vec3(mesh->mVertices[mesh_iter].x, mesh->mVertices[mesh_iter].y, mesh->mVertices[mesh_iter].z);

		if (mesh->mNormals) { vertex.normal = vec3(mesh->mNormals[mesh_iter].x, mesh->mNormals[mesh_iter].y, mesh->mNormals[mesh_iter].z); }
		else { vertex.normal = vec3(0.0f); }
        
        if (mesh->mTextureCoords[0]){
            // Index at 0 before mesh_iter because Assimp allows for UP TO 8 TEXTURES for A SINGLE VERTEX
            // mesh->mTextureCoords[1], etc
            vertex.texture_coords = vec2(mesh->mTextureCoords[0][mesh_iter].x, mesh->mTextureCoords[0][mesh_iter].y);

        } else { vertex.texture_coords = vec2(0.0f); }
        mesh_vertices.push_back(vertex);
    }
    
    for (size_t face_iter = 0; face_iter < mesh->mNumFaces; ++face_iter){
        for (size_t index_iter = 0; index_iter < mesh->mFaces[face_iter].mNumIndices; ++index_iter){
            vertex_indices.push_back(mesh->mFaces[face_iter].mIndices[index_iter]);
        }
    }
    
    // Process material
    if(scene->mNumMaterials > 0){
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        
		load_texture(textures, material, aiTextureType_DIFFUSE, "texture_diffuse");
		load_texture(textures, material, aiTextureType_SPECULAR, "texture_specular");
    }
    
    return Mesh(mesh_vertices, vertex_indices, textures);
}

void Model::load_texture(std::vector<MeshTexture>& load_vector, aiMaterial* material, aiTextureType type, const std::string& type_name){
    if (material->GetTextureCount(type) > 0){
        for (size_t texture_count = 0; texture_count < material->GetTextureCount(type); ++texture_count){
            aiString path;
            material->GetTexture(type, static_cast<unsigned int>(texture_count), &path);
            
            bool skip = false;
            for (size_t texture_iter = 0; texture_iter < Model::loaded_textures.size(); ++texture_iter){
				if (Model::loaded_textures.at(texture_iter).texture_type != MeshTexture::TextureType::TEXTURE) { continue; }
                if (Model::loaded_textures.at(texture_iter).path == path){
                    load_vector.push_back(Model::loaded_textures.at(texture_iter));
                    skip = true; break;
                }
            }
            
            if (!skip) {
                // Texture not already loaded
                MeshTexture texture;
				texture.texture_type = MeshTexture::TextureType::TEXTURE;
                texture.id = Shape::load_texture_from_file(path.C_Str(), model_directory);
                texture.type = type_name;
                texture.path = path;
                load_vector.push_back(texture);
                
                // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
                Model::loaded_textures.push_back(texture);
            }
        }

	} else {	// Texture is coloured component
		std::string const_type = (type == aiTextureType_DIFFUSE) ? "$clr.specular" : (type == aiTextureType_SPECULAR) ? "$clr.specular" : "-1";

		if (const_type == "-1") { return; }

		aiColor4D colour_value;
		if (AI_SUCCESS == aiGetMaterialColor(material, const_type.c_str(), 0, 0, &colour_value)) {
			MeshTexture new_tex;
			new_tex.texture_type = MeshTexture::TextureType::COLOUR;
			new_tex.id = Shape::load_texture_from_rgba(colour_value.r, colour_value.g, colour_value.b, colour_value.a);
			new_tex.type = type_name;
			new_tex.colour = colour_value;
			load_vector.push_back(new_tex);
		}
	}
}

void Model::render(const SHADER_ID& id){
    evaluate_changed();
    
    for (size_t mesh_index = 0; mesh_index < meshes.size(); mesh_index++){
        meshes.at(mesh_index).render(id);
    }
}

void Model::evaluate_changed(){
	if (matrix_needs_update) { _needs_evaluation = true; }
    if (!_needs_evaluation){ return; }
    _needs_evaluation = false;
        
    for (size_t mesh_iter = 0; mesh_iter < meshes.size(); mesh_iter++){
        meshes.at(mesh_iter).set_rotation_point(this->rotation_point);
        meshes.at(mesh_iter).set_position(this->translation_vector);
        meshes.at(mesh_iter).set_move_vector(this->move_vector);
        meshes.at(mesh_iter).set_enlargement(this->enlargement);
        meshes.at(mesh_iter).set_quaternion(this->quaternion);
        meshes.at(mesh_iter).evaluate_changed();
    }
}

std::vector<vec3> Model::get_personal_vertices() {
	std::vector<vec3> return_verts;

	for (size_t i = 0; i < meshes.size(); ++i) {
		Mesh mesh = meshes.at(i);

		for (size_t j = 0; j < mesh.get_vertices_size(); ++j) {
			return_verts.push_back(mesh.get_vertex(j).position);
		}
	}

	return return_verts;
}
