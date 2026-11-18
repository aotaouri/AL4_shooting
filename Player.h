#pragma once
#include "KamataEngine.h"
#include <list>

using namespace KamataEngine;

class PlayerBullet;

class Player {

public:

	~Player();

	void  Initialize(KamataEngine::Model* model, KamataEngine::Model* modelBullet, KamataEngine::Camera* camera, const Vector3& position);

	void Update();

	void Draw();

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const Vector3& GetVelocity() const { return velocity_; }


private:

	// 弾を撃つ
	void Attack();

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Model* modelBullet_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	// 速度(1フレームあたりの移動量)
	Vector3 velocity_ = {};

	Vector3 aimDirection_ = {0.0f, 0.0f, 1.0f};

	std::list<PlayerBullet*> bullets_;

	// 移動速度
	static inline const float kMoveSpeed = 0.2f;

	static inline const float kBulletSpeed = 0.5f;

};
