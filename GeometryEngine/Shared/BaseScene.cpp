#include "BaseScene.hpp"


BaseScene::BaseScene(WindowWrapper* window, Camera* camera) : window(window), camera(camera) {
	held_keys.fill(false);
}

BaseScene::~BaseScene() {
	MemoryManagement::delete_all_from_vector(renderables);
}
