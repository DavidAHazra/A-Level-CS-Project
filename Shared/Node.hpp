#pragma once

#include "GameHelpers.hpp"


class Node {
public:
    //Node() = delete;
	//Node(const Node& other) = delete;
	//void operator=(const Node& other) = delete;
    
    Node(int x, int y) : x(x), y(y) {}
    
    inline int get_x() const { return x; }
    inline int get_y() const { return y; }
    
    inline int get_f() { return is_wall ? 3 * (INT_MAX / 4) : g + h; }
    
    inline void set_g(int new_g) { g = new_g; }
    inline void set_h(int new_h) { h = new_h; }
    inline void set_wall(bool new_is_wall) { is_wall = new_is_wall; }
    inline bool get_is_wall() { return is_wall; }

    inline void set_parent(Node* new_parent){ parent = new_parent; }
    inline Node* get_parent() { return parent; }
    
    int g = INT_MAX / 3;
    int h = INT_MAX / 3;

	void reset();
    
private:
    int x;
    int y;

    // Parent is node with most efficient path
    Node* parent = nullptr;
    bool is_wall = false;
};

inline bool NodeComparison(Node* A, Node* B) {
	return A->get_f() < B->get_f();
}
