#pragma once
#include "kamataEngine.h"

using namespace KamataEngine;

class Item {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position);

	bool Update(const Vector3& playerPos, float deltaTime);

	void Draw();

	bool IsAlive() const { return isAlive_; }

	const Vector3& GetPosition() const { return worldTransform_.translation_; }

private:

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	bool isAlive_ = true;
	bool isAttracting_ = false;

	// 吸い寄せ中の現在速度
	float speed_ = 0.0f;

	// 吸引範囲
	static inline const float kAttractRange = 8.0f;
	// 取得範囲
	static inline const float kPickupRange = 1.0f;
	// 加速度
	static inline const float kAcceleration = 30.0f;
};
