#pragma once
#include "Shape.hpp"

struct MeshVertex {
    vec3 position;
    vec3 normal;
    vec2 texture_coords;
};

struct MeshTexture {
	enum TextureType {
		TEXTURE,
		COLOUR
	} texture_type;

    int id;
    std::string type;

	// Depending on what kind of texture this is, either path or colour could be uninitialised
	aiString path;
	aiColor4D colour;
};


class Mesh : public Shape /* Not colourable, slightly more complex */ {
public:
	Mesh(const std::vector<MeshVertex>& mesh_vertices, const std::vector<UInt>& indices, const std::vector<MeshTexture>& textures);
	Mesh(const Mesh& other);
	void operator=(const Mesh& other);

	virtual ~Mesh() {}
    
    virtual void render(const SHADER_ID& id = Mesh::GENERIC_ID());
    virtual void evaluate_changed();
    
    inline MeshVertex get_vertex(const size_t& index) const { return mesh_vertices.at(index); }
    inline size_t get_vertices_size() const { return mesh_vertices.size(); }
    
    inline void set_vertices(const std::vector<MeshVertex>& new_vertices)	{ mesh_vertices = new_vertices; _needs_evaluation = true; }
    inline void set_indices(const std::vector<UInt>& new_indices)			{ indices = new_indices; _needs_evaluation = true; }
    inline void set_textures(const std::vector<MeshTexture>& new_textures)	{ textures = new_textures; _needs_evaluation = true; }
    
private:
    std::vector<MeshVertex> mesh_vertices;
    std::vector<UInt> indices;
    std::vector<MeshTexture> textures;
    
    UInt EBO;
};

