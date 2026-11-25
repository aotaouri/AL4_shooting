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

	// 座標を外から動かす(押し合いの反映用)
	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; }

	// 座標を変えたあとに行列を更新する
	void RefreshTransform();

	// 当たり判定の半径(GameScene から参照する)
	static inline const float kRadius = 0.8f;

	private:
	// プレイヤーを検知できるか(XZ平面の距離で判定)
	bool CanDetectPlayer(const Vector3& enemyPos, const Vector3& playerPos, float detectRange) const;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	// 追跡中か
	bool isChasing_ = false;

	// 索敵範囲
	static inline const float kDetectRange = 14.0f;

	// 移動速度
	static inline const float kMoveSpeed = 0.08f;

	// 一度追跡を始めたら、画面外でも追い続けるか
	bool hasNoticed_ = false;
};
