#include "PlayerBullet.h"
#include "MyMath.h"

using namespace KamataEngine;

void PlayerBullet::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position, const Vector3& velocity) {

	model_ = model;
	camera_ = camera;
	velocity_ = velocity;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
}

void PlayerBullet::Update() {

	// 寿命が尽きたら消す
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	// 座標を移動させる
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	UpdateWorldTransform(worldTransform_);
}

void PlayerBullet::Draw()
{ 
	model_->Draw(worldTransform_, *camera_);
}