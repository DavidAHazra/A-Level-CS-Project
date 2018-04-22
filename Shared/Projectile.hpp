#pragma once
#include "SAT_OBB.hpp"
#include "Cube.hpp"
#include "Model.hpp"


enum Abilities {
    POTATO,
    FIREBALL,
    ICEBALL,
    MAGIC,
    
    DEFAULT_ABILITY = POTATO,
    FIRST_ABILITY = POTATO,
    LAST_ABILITY = MAGIC
};


class Projectile {
public:
    Projectile() = delete;
    Projectile(const Projectile& other) = delete;
    void operator=(const Projectile& other) = delete;
    
public:
    virtual ~Projectile() {}
    
    void update(const float& time_delta);
    virtual void render();
    
    Cube* get_projectile_cube() { return &projectile_cube; }
    Collidable get_collidable() { return Collidable::make_collidable(projectile_cube); }
	virtual Abilities get_type() = 0;
    
protected:
    Projectile(const vec3& movement_vector, const float& speed);
    Cube projectile_cube;
};

// ==== ==== ==== ====
// Basic Projectile
// ==== ==== ==== ====

class Potato : public Projectile {
public:
    Potato(const Potato& other) = delete;
    void operator=(const Potato& other) = delete;
    
    virtual void render();
	inline virtual Abilities get_type() { return Abilities::POTATO; }
    
public:
    Potato(const vec3& movement_vector);
    virtual ~Potato() {}
    
    static const UInt mana_cost;
    static const UInt damage;
    static const float speed;
    
private:
    Model potato;
};

// ==== ==== ==== ====
// Fireball
// ==== ==== ==== ====

class FireBall : public Projectile {
public:
    FireBall(const FireBall& other) = delete;
    void operator=(const FireBall& other) = delete;

	inline virtual Abilities get_type() { return Abilities::FIREBALL; }
    
public:
    FireBall(const vec3& movement_vector);
    virtual ~FireBall() {}
    
    static const UInt mana_cost;
    static const UInt damage;
    static const float speed;
};


// ==== ==== ==== ====
// Iceball
// ==== ==== ==== ====

class IceBall : public Projectile {
public:
    IceBall(const IceBall& other) = delete;
    void operator=(const IceBall& other) = delete;

	inline virtual Abilities get_type() { return Abilities::ICEBALL; }
    
public:
    IceBall(const vec3& movement_vector);
    virtual ~IceBall() {}
    
    static const UInt mana_cost;
    static const UInt damage;
    static const float speed;
};


// ==== ==== ==== ====
// Rapid Magic
// ==== ==== ==== ====

class Magic : public Projectile {
public:
    Magic(const Magic& other) = delete;
    void operator=(const Magic& other) = delete;

	inline virtual Abilities get_type() { return Abilities::MAGIC; }
    
public:
    Magic(const vec3& movement_vector);
    virtual ~Magic() {}
    
    static const UInt mana_cost; // Spawns 5 at a time therefore actually costs 15 mana
    static const UInt damage;
    static const float speed;
};
