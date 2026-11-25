#pragma once
#include "KamataEngine.h"
#include <list>

using namespace KamataEngine;

class PlayerBullet;

class Stage;

class Player {

public:

	~Player();

	void  Initialize(KamataEngine::Model* model, KamataEngine::Model* modelBullet, KamataEngine::Camera* camera, const Vector3& position);

	void Update();

	void Draw();

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const Vector3& GetVelocity() const { return velocity_; }

	// 壁との衝突で押し戻された座標を反映する
	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; }

	void RefreshTransform();

		// 壁に触れた弾を消す
	void RemoveBulletsHitWall(const Stage& stage);

	void LookAt(const Vector3& target);

	// 弾と敵の当たり判定用:敵の座標と半径を渡して、当たった弾を消す。当たったら true
	bool HitBullet(const Vector3& enemyPos, float enemyRadius);

	// 当たり判定の半径
	static inline const float kRadius = 0.8f;

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
