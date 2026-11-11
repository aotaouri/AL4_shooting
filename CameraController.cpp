#include "CameraController.h"
#include "Player.h"
#include <cmath>
#include <numbers>

using namespace KamataEngine;

// 線形補間
static float Lerp(float start, float end, float t) { return start + (end - start) * t; }


void CameraController::Initialize() {

	camera_.Initialize();

	// X軸まわりに90°(π/2)回転させて真下を向ける
	camera_.rotation_ = {std::atan2(targetOffset_.y, -targetOffset_.z), 0.0f, 0.0f};

}

void CameraController::Reset() {
	const WorldTransform& targetWT = target_->GetWorldTransform();

	// 目標座標を求め、カメラを直接そこへ合わせる(飛んでくるのを防ぐ)
	destination_.x = targetWT.translation_.x + targetOffset_.x;
	destination_.y = targetWT.translation_.y + targetOffset_.y;
	destination_.z = targetWT.translation_.z + targetOffset_.z;

	camera_.translation_ = destination_;

	camera_.UpdateMatrix();
}

void CameraController::Update() {
	const WorldTransform& targetWT = target_->GetWorldTransform();

	// 目標座標を求める
	destination_.x = targetWT.translation_.x + targetOffset_.x;
	destination_.y = targetWT.translation_.y + targetOffset_.y;
	destination_.z = targetWT.translation_.z + targetOffset_.z;

	// 現在座標を目標座標へ補間
	camera_.translation_.x = Lerp(camera_.translation_.x, destination_.x, kInterpolationRate);
	camera_.translation_.y = Lerp(camera_.translation_.y, destination_.y, kInterpolationRate);
	camera_.translation_.z = Lerp(camera_.translation_.z, destination_.z, kInterpolationRate);

	camera_.UpdateMatrix();
}