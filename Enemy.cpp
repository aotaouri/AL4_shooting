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

	// 気づく条件(画面内 → 距離 → 視線)。一度気づいたら追い続ける
	if (!hasNoticed_) {
		bool onScreen = IsInsideScreen(worldTransform_.translation_, viewProjection);
		hasNoticed_ = onScreen && CanDetectPlayer(worldTransform_.translation_, playerPos, kDetectRange) && stage.HasLineOfSight(worldTransform_.translation_, playerPos);
	}
	isChasing_ = hasNoticed_;

	if (isChasing_) {
		Vector3 dir;
		if (stage.GetFlowDirection(worldTransform_.translation_, &dir)) {
			// 自分のいるマスのベクトルを参照して進むだけ
			worldTransform_.translation_.x += dir.x * kMoveSpeed;
			worldTransform_.translation_.z += dir.z * kMoveSpeed;
			worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
		} else if (stage.HasLineOfSight(worldTransform_.translation_, playerPos)) {
			float diffX = playerPos.x - worldTransform_.translation_.x;
			float diffZ = playerPos.z - worldTransform_.translation_.z;
			float length = std::sqrt(diffX * diffX + diffZ * diffZ);
			if (length > 0.0001f) {
				worldTransform_.translation_.x += diffX / length * kMoveSpeed;
				worldTransform_.translation_.z += diffZ / length * kMoveSpeed;
				worldTransform_.rotation_.y = std::atan2(diffX, diffZ);
			}
		}
		// 壁にめり込んだ分を押し戻す(追加)
		worldTransform_.translation_ = stage.ResolveCollision(worldTransform_.translation_, kRadius);
	}

	UpdateWorldTransform(worldTransform_);
}

void Enemy::RefreshTransform() { UpdateWorldTransform(worldTransform_); }

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }