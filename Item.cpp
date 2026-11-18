#include "Item.h"
#include "MyMath.h"
#include <cmath>

using namespace KamataEngine;

void Item::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position) {

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// 生成直後のフレームから正しい位置に描画されるようにする
	UpdateWorldTransform(worldTransform_);
}

bool Item::Update(const Vector3& playerPos, float deltaTime) {

	if (!isAlive_) {
		return false;
	}

	// 1. XZ平面の差分から距離を求める
	float diffX = playerPos.x - worldTransform_.translation_.x;
	float diffZ = playerPos.z - worldTransform_.translation_.z;
	float distance = std::sqrt(diffX * diffX + diffZ * diffZ);

	// 2. 吸引範囲に入ったら吸い寄せ開始(一度入ったら範囲外に出ても続ける)
	if (distance <= kAttractRange) {
		isAttracting_ = true;
	}

	// 3. 取得範囲に入ったら消して true を返す
	if (distance <= kPickupRange) {
		isAlive_ = false;
		return true; // 取得完了(2D演出の発生へ)
	}

	// 4. 吸引中なら加速しながらプレイヤーへ近づく
	if (isAttracting_ && distance > 0.0f) {
		speed_ += kAcceleration * deltaTime;
		worldTransform_.translation_.x += (diffX / distance) * speed_ * deltaTime;
		worldTransform_.translation_.z += (diffZ / distance) * speed_ * deltaTime;
	}

	// 見つけやすいように回転させておく(不要なら削除)
	worldTransform_.rotation_.y += 0.05f;

	UpdateWorldTransform(worldTransform_);
	return false;
}

void Item::Draw()
{ 

	model_->Draw(worldTransform_, *camera_);

}
