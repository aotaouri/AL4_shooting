#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Stage;

class Enemy {

public:

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position);

	// プレイヤーの座標を受け取って更新する
	void Update(const Vector3& playerPos, const KamataEngine::Matrix4x4& viewProjection, const Stage& stage);

	void Draw();

	const Vector3& GetPosition() const { return worldTransform_.translation_; }

	// 索敵中かどうか(確認用)
	bool IsChasing() const { return isChasing_; }

	private:
	// プレイヤーを検知できるか(XZ平面の距離で判定)
	bool CanDetectPlayer(const Vector3& enemyPos, const Vector3& playerPos, float detectRange) const;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	// 追跡中か
	bool isChasing_ = false;

	// 索敵範囲
	static inline const float kDetectRange = 8.0f;

	// 移動速度
	static inline const float kMoveSpeed = 0.08f;


};
