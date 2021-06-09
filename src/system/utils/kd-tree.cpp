#include "kd-tree.hpp"


KD_Tree::KD_Tree(bool isEnemy) {
    if(isEnemy){
        for(auto entity: m_registry.view<Enemy>()){
            this->root = insert(this->root, entity);
        }
    }else{
        for(auto entity: m_registry.view<Ally>()){
            this->root = insert(this->root, entity);
        }
    }
}

void KD_Tree::print(KD_Node* root) {
	if (root) {
		Position& positionEnt = m_registry.get<Position>(root->e);
		std::cout << "(" << positionEnt.position.x << ", " << positionEnt.position.y << ") " << std::endl;
		std::cout << "left" << std::endl;
		print(root->left);
		std::cout << "right" << std::endl;
		print(root->right);
	}
}


KD_Tree::KD_Node* KD_Tree::insert(KD_Node*& root, entt::entity e, int depth) {
	if (root != nullptr) {
		Position& positionEnt = m_registry.get<Position>(e);
        Position& positionRoot = m_registry.get<Position>(root->e);
		if (depth % 2) {
			if (positionEnt.position.x < positionRoot.position.x) {
				root->left = insert(root->left, e, depth++);
			}
			else {
				root->right = insert(root->right, e, depth++);
			}
		}
		else {
			if (positionEnt.position.y < positionRoot.position.y) {
				root->left = insert(root->left, e, depth++);
			}
			else {
				root->right = insert(root->right, e, depth++);
			}
		}
	}
	else {
		root = new KD_Node(e);
	}
	return root;
}

void KD_Tree::nearest(KD_Node*& root, entt::entity e, entt::entity& res, float min, int depth) {
	if (root == nullptr) {
		return;
	}
	float d = dist(root->e, e);
	if (d >= min) {
		vec2 pose = m_registry.get<Position>(e).position;
		vec2 posr = m_registry.get<Position>(root->e).position;
		if (depth % 2) {
			if (pose.x < posr.x) {
				nearest(root->left, e, res);
			}
			else {
				nearest(root->right, e, res);
			}
		}
		else {
			if (pose.y < posr.y) {
				nearest(root->left, e, res);
			}
			else {
				nearest(root->right, e, res);
			}
		}
		return;
	}
	else {
		res = root->e;
		min = d;
		nearest(root->left, e, res, min, depth++);
		nearest(root->right, e, res, min, depth++);
	}

}