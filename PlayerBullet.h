#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class PlayerBullet {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position, const Vector3& velocity);

	void Update();

	void Draw();

	bool IsDead() const { return isDead_; }

	const Vector3& GetPosition() const { return worldTransform_.translation_; }

private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	// 速度(1フレームあたりの移動量)
	Vector3 velocity_ = {};

	// 寿命(フレーム数)
	static inline const int32_t kLifeTime = 60 * 2;

	// 経過フレーム
	int32_t deathTimer_ = kLifeTime;

	bool isDead_ = false;
};