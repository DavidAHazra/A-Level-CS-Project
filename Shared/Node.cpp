#include "Node.hpp"


void Node::reset() {
	h = INT_MAX / 3;
    g = INT_MAX / 3;
	parent = nullptr;
}
