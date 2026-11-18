#include "Enemy.h"
#include "MyMath.h"
#include "Stage.h"
#include <cmath>

using namespace KamataEngine;

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position) {

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	UpdateWorldTransform(worldTransform_);
}

bool Enemy::CanDetectPlayer(const Vector3& enemyPos, const Vector3& playerPos, float detectRange) const {

	// XZ平面の差分を求める
	float diffX = playerPos.x - enemyPos.x;
	float diffZ = playerPos.z - enemyPos.z;

	// ベクトルの長さ(距離)を求めて比較する
	float distance = std::sqrt(diffX * diffX + diffZ * diffZ);
	return distance <= detectRange;
}

void Enemy::Update(const Vector3& playerPos, const Matrix4x4& viewProjection, const Stage& stage) {

	// 軽い判定から順に行う(画面内 → 距離 → 壁に遮られていないか)
	bool onScreen = IsInsideScreen(worldTransform_.translation_, viewProjection);
	isChasing_ = onScreen && CanDetectPlayer(worldTransform_.translation_, playerPos, kDetectRange) && stage.HasLineOfSight(worldTransform_.translation_, playerPos);

	if (isChasing_) {
		// プレイヤーへの方向を求める
		float diffX = playerPos.x - worldTransform_.translation_.x;
		float diffZ = playerPos.z - worldTransform_.translation_.z;
		float length = std::sqrt(diffX * diffX + diffZ * diffZ);

		// 重なっているとき(長さ0)は割らない
		if (length > 0.0001f) {
			worldTransform_.translation_.x += diffX / length * kMoveSpeed;
			worldTransform_.translation_.z += diffZ / length * kMoveSpeed;

			// 進行方向を向く(Y軸回転)
			worldTransform_.rotation_.y = std::atan2(diffX, diffZ);
		}
	}

	UpdateWorldTransform(worldTransform_);
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }