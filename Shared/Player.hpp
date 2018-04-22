#pragma once
#include "Character.hpp"
#include "Camera.hpp"
#include "Rectangle.hpp"
#include "Projectile.hpp"
#include "Text.hpp"
#include "Node.hpp"


class Player : public Character {
public:
    Player() = delete;
    Player(const Player& other) = delete;
    void operator=(const Player& other) = delete;
    
public:
	Player(const float& width, const float& height, const float& depth, Camera* player_camera);
    virtual ~Player();

	void move(float right, float forward);
	
    virtual void handle_mouse(const float& x_pos, const float& y_pos);
    virtual void update(const float& time_delta);
    
    void increment_score();
    
    void render();
    void fire_ability();
    void next_ability();
    
    vec3 get_front_vector();

	void enemy_hit();
    
    inline UInt get_score() { return score; }

private:
	Camera* camera;
    Rect vertical_crosshair;
    Rect horisontal_crosshair;
    std::vector<Projectile*> projectiles;
    
    Rect potato_icon;
    Rect fireball_icon;
    Rect iceball_icon;
    Rect magic_icon;
    
    Text potato_text;
    Text fireball_text;
    Text iceball_text;
    Text magic_text;
    
    Text mana_text;
    Text health_text;
    Text score_text;
    
    Abilities current_ability = Abilities::POTATO;
    
    bool first_person = true;
    const float camera_distance = 15.0f;
    
    UInt mana = 100;
    float mana_counter = 0.0f;
    UInt mana_gain = 5;
    
    int health = 100;
    UInt score = 0;
    
	inline bool has_enough_for_cast() { return Player::has_enough_for_cast(mana, current_ability); }
	static bool has_enough_for_cast(const UInt& mana, const Abilities& ability);

public:
    // Inline getters / setters
    inline void toggle_first_person() { first_person = !first_person; }
    inline std::vector<Projectile*>& get_projectiles() { return projectiles; }
	inline int get_health() { return health; }
};
