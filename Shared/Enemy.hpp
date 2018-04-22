#pragma once
#include "Character.hpp"
#include "Map.hpp"
#include "Text.hpp"
#include "Projectile.hpp"

class Enemy : public Character {
    const float burn_timeout = 1.5f;
    const float ice_timeout = 5.0f;
public:
    Enemy(const Enemy& other) = delete;
    void operator=(const Enemy& other) = delete;

	static const float PATH_REFRESH_TIME;
    
public:
    Enemy();
    virtual ~Enemy();
    
    virtual void update(const float& time_delta);
    virtual void render();
    
    void move_towards_closest_node(Map& map);
    
	void set_renderable(Renderable* new_renderable, bool dynamic_object);
    
    inline void explode() { do_explode = true; }
    
    inline float get_time_exploding(){ return time_exploding; }
    inline bool get_is_exploding() const { return do_explode; }
    inline bool get_is_done() const { return is_done; }
    inline void set_shortest_path(const std::vector<Node*>& shortest) { path = shortest; node_number = 1; }
    inline void set_is_done(const bool new_done) { is_done = new_done; }
    
    inline float get_path_counter() { return path_counter; }
    inline void add_count(const float& time_delta) { path_counter += time_delta; }

	void rotate_text_towards_position(const vec3& pos);

	void hit(const Abilities& projectile_type);

private:
    Renderable* renderable;
    bool dynamic = false;
    bool do_explode = false;
    bool is_done = false;
        
    float fire_timer = burn_timeout;
    float ice_timer = ice_timeout;
    
    float time_exploding = 0.0f;

	int node_number = 1;
    std::vector<Node*> path;
    Node* top_node = nullptr;
    vec3 target;
    float path_counter = 0.0f;

	int health = 100;
	Text health_text;
	vec3 text_normal = vec3(0.0f, 0.0f, 1.0f);

	Node* get_next_node(Map& map);
};
