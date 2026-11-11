#include "Player.h"
#include "MyMath.h"
#include <cmath>

using namespace KamataEngine;

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position) {

	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
}

void Player::Update() {

	Input* input = Input::GetInstance();

	// 入力から移動方向を決める(X・Z 平面)
	Vector3 direction = {0.0f, 0.0f, 0.0f};

	if (input->PushKey(DIK_W)) {
		direction.z += 1.0f;
	}
	if (input->PushKey(DIK_S)) {
		direction.z -= 1.0f;
	}
	if (input->PushKey(DIK_D)) {
		direction.x += 1.0f;
	}
	if (input->PushKey(DIK_A)) {
		direction.x -= 1.0f;
	}

	// 斜め移動が速くならないよう正規化する
	float length = std::sqrt(direction.x * direction.x + direction.z * direction.z);
	if (length > 0.0f) {
		direction.x /= length;
		direction.z /= length;
	}

	// 速度を求めて座標に加算
	velocity_.x = direction.x * kMoveSpeed;
	velocity_.y = 0.0f;
	velocity_.z = direction.z * kMoveSpeed;

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.z += velocity_.z;


	// 行列を更新して転送
	UpdateWorldTransform(worldTransform_);

}

void Player::Draw() {



	model_->Draw(worldTransform_, *camera_);



}
