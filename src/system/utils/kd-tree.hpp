
#ifndef KD_TREE_H
#define KD_TREE_H


#include "core/common.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include "core/registry.hpp"
#include "components/motion.hpp"
#include "components/units.hpp"

class KD_Tree {
public:
    KD_Tree(bool isEnemy);

    struct KD_Node {
		entt::entity e;
		KD_Node* left, * right;

		KD_Node(entt::entity e, KD_Node* left = nullptr, KD_Node* right = nullptr) {
			this->e = e;
			this->left = left;
			this->right = right;
		}

	};

	KD_Node* root = nullptr;

	KD_Node* insert(KD_Node*& root, entt::entity e, int depth = 0);

	void nearest(KD_Node*& root, entt::entity e, entt::entity& res, float min = INFINITY, int depth = 0);

	void print(KD_Node*);

	float dist(entt::entity root, entt::entity e) {
		vec2 pose = m_registry.get<Position>(e).position;
		vec2 posr = m_registry.get<Position>(root).position;
		return pow(pose.x - posr.x, 2) + pow(pose.y - posr.y, 2);
	}



};

#endif