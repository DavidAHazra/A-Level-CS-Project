#pragma once

#include "Node.hpp"

class NodeQueue : public std::vector<Node*> {
public:
	NodeQueue(Node* node) : std::vector<Node*>(std::vector<Node*> { node }) {}

	void sort_insert(Node* node) {
		std::vector<Node*>::iterator iter = std::upper_bound(this->begin(), this->end(), node, NodeComparison);
		this->insert(iter, node);
	}

	void manual_sort() {
		std::sort(this->begin(), this->end(), NodeComparison);
	}
};


class Map {
public:
    Map() {}
    Map(const std::vector<std::vector<Node*>>& node_map) : node_map(node_map) {}
    Map(const Map& other) : node_map(other.node_map) {}
    void operator=(const Map& other) { node_map = other.node_map; }
    
    virtual ~Map();
    
    std::vector<Node*> a_star_pathfind(Node* start_node, Node* end_node);
    
    unsigned int heuristic_estimate(Node* start_node, Node* end_node);
    std::vector<Node*> reconstruct_path(Node* current);
    std::vector<Node*> get_node_neighbours(Node* current);
    std::vector<Node*> get_all_nodes();
    std::pair<size_t, size_t> get_location_of_node_in_map(Node* current);
	void reset();
    int movement_cost(Node* start, Node* end);
    Node* get_closest_node(const vec3& position);
    
    inline Node* get_node(size_t x, size_t y){ return node_map.at(x).at(y); }
    inline size_t get_node_map_size() { return node_map.size(); }
    inline size_t get_node_column_size(const size_t x) { return node_map.at(x).size(); }
    
private:
    std::vector<std::vector<Node*>> node_map;
};
