#include "Map.hpp"
#include <algorithm>


Map::~Map(){
    for (size_t i = 0; i < node_map.size(); ++i){
        for (size_t j = 0; j < node_map.at(i).size(); ++j){
			delete node_map[i][j];
			node_map[i][j] = nullptr;
        }
        
        node_map[i].clear();
    }
    
    node_map.clear();
}

std::vector<Node*> Map::a_star_pathfind(Node* start_node, Node* end_node){
    if (start_node->get_is_wall() || end_node->get_is_wall()) { return std::vector<Node*>(); }
    
    this->reset();
    
	NodeQueue open_set(start_node);
	std::vector<Node*> closed_set;
        
    start_node->set_g(0);
    start_node->set_h(heuristic_estimate(start_node, end_node));
    
    while (!open_set.empty()) {
		open_set.manual_sort();
		Node* current = open_set.at(0);
        
        if (current->get_is_wall()){
            closed_set.push_back(current);
			open_set.erase(std::remove(open_set.begin(), open_set.end(), current), open_set.end());
			continue;
        }
        
        if (current == end_node) {
            return reconstruct_path(current);
        }
        
		open_set.erase(std::remove(open_set.begin(), open_set.end(), current), open_set.end());
        closed_set.push_back(current);
        
        std::vector<Node*> current_neighbours = get_node_neighbours(current);
        for (size_t i = 0; i < current_neighbours.size(); ++i){
            Node* neighbour = current_neighbours.at(i);
            
            if (std::find(closed_set.begin(), closed_set.end(), neighbour) != closed_set.end()){
                // Neighbour is in closed set - ignore it
                continue;
            }

			if (std::find(open_set.begin(), open_set.end(), neighbour) == open_set.end()){
                neighbour->set_h(heuristic_estimate(neighbour, end_node));
				open_set.push_back(neighbour);
            }
            
			int test_g = current->g + movement_cost(current, neighbour);
            if (test_g >= neighbour->g){
                continue;
            }
            
            neighbour->set_parent(current);
            neighbour->set_g(test_g);
        }
    }
    
    return std::vector<Node*>();
}

int Map::movement_cost(Node* start, Node* end) {
    // This function is defined only so that if in
    // the future I ever want to add diagonal movement it is easier
    return 1;
}

unsigned int Map::heuristic_estimate(Node* start_node, Node* end_node){
    int delta_x = end_node->get_x() - start_node->get_x();
	int delta_y = end_node->get_y() - start_node->get_y();
    
    return static_cast<int>(std::sqrt(delta_x * delta_x + delta_y * delta_y));
}

std::vector<Node*> Map::reconstruct_path(Node* current){
    std::vector<Node*> finished_path = { current };
    
    Node* iter = current;
    
    while (iter->get_parent()){
        iter = iter->get_parent();
        finished_path.push_back(iter);
    }
    
    std::reverse(finished_path.begin(), finished_path.end());
    
    return finished_path;
}

std::vector<Node*> Map::get_node_neighbours(Node* current){
    std::pair<size_t, size_t> node_coords = get_location_of_node_in_map(current);
    
    std::vector<Node*> node_neighbours;
    
    size_t x = node_coords.first;
    size_t y = node_coords.second;
    
    // Checking node boundaries
    if (x > 0){
        Node* left_node = node_map.at(x - 1).at(y);
        node_neighbours.push_back(left_node);
    }
    
    if (x < (node_map.size() -1)){
        Node* right_node = node_map.at(x + 1).at(y);
        node_neighbours.push_back(right_node);
    }

    if (y > 0){
        Node* up_node = node_map.at(x).at(y - 1);
        node_neighbours.push_back(up_node);
    }
    
    if (y < (node_map.at(x).size() - 1)){
        Node* down_node = node_map.at(x).at(y + 1);
        node_neighbours.push_back(down_node);
    }
    
    return node_neighbours;
}

std::pair<size_t, size_t> Map::get_location_of_node_in_map(Node* current){
    bool found_current = false;
    std::pair<size_t, size_t> coordinates;
    
    for (size_t x = 0; x < node_map.size(); ++x){
        std::vector<Node*> column = node_map.at(x);
        
        for (size_t y = 0; y < column.size(); ++y){
            Node* node = column.at(y);
            if (node == current){
                found_current = true;
                coordinates = { x, y };
                break;
            }
        }
        
        if (found_current) { break; }
    }
    
    if (!found_current){
        throw std::runtime_error("Could not find current node in map");
    }
    
    return coordinates;
}

void Map::reset() {
	for (size_t i = 0; i < node_map.size(); ++i) {
		for (size_t j = 0; j < node_map.at(i).size(); ++j) {
			node_map.at(i).at(j)->reset();
		}
	}
}

std::vector<Node*> Map::get_all_nodes() {
    std::vector<Node*> return_list;
    
    for (size_t x = 0; x < node_map.size(); ++x){
        for (size_t y = 0; y < node_map.at(x).size(); ++y){
            return_list.push_back(node_map.at(x).at(y));
        }
    }
    
    return return_list;
}

Node* Map::get_closest_node(const vec3& position) {
    vec2 c = { position.x, position.z };
    Node* closest = nullptr;
    
    for (size_t i = 0; i < get_node_map_size(); ++i){
        for (size_t j = 0; j < get_node_column_size(i); ++j){
            Node* current = node_map.at(i).at(j);
            if (!closest) { closest = current; continue; }
            vec2 node_pos = { static_cast<float>(current->get_x()), static_cast<float>(current->get_y()) };
            
            auto dist = distance(c, node_pos);
            if (dist < distance(c, { static_cast<float>(closest->get_x()), static_cast<float>(closest->get_y()) })){
                closest = current;
            }
        }
    }
    
    return closest;
}
