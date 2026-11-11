#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player {

public:

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position);

	void Update();

	void Draw();

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

private:

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	// 速度(1フレームあたりの移動量)
	Vector3 velocity_ = {};

	// 移動速度
	static inline const float kMoveSpeed = 0.2f;

};
