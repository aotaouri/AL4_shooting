#include "Player.h"
#include "MyMath.h"
#include "PlayerBullet.h"
#include <cmath>

using namespace KamataEngine;

Player::~Player()
{
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	bullets_.clear();
}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Model* modelBullet, KamataEngine::Camera* camera, const Vector3& position) {

	model_ = model;
	modelBullet_ = modelBullet;
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

		aimDirection_ = direction;

		// 移動方向へ滑らかに向く(追加)
		float targetAngle = std::atan2(direction.x, direction.z);

		// 角度の差を -π〜π に収める(遠回りして回るのを防ぐ)
		float diff = std::remainder(targetAngle - worldTransform_.rotation_.y, 2.0f * 3.1415926535f);

		// 差の20%ずつ近づける(1.0fにすると一瞬で向く)
		worldTransform_.rotation_.y += diff * 0.2f;
	}

	// 速度を求めて座標に加算
	velocity_.x = direction.x * kMoveSpeed;
	velocity_.y = 0.0f;
	velocity_.z = direction.z * kMoveSpeed;

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.z += velocity_.z;

	// スペースキーで弾を撃つ
	if (input->TriggerKey(DIK_SPACE)) {
		Attack();
	}

	// 弾の更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 死んだ弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 行列を更新して転送
	UpdateWorldTransform(worldTransform_);

}

void Player::Attack() {

	Vector3 velocity = {aimDirection_.x * kBulletSpeed, 0.0f, aimDirection_.z * kBulletSpeed};

	PlayerBullet* newBullet = new PlayerBullet();
	newBullet->Initialize(modelBullet_, camera_, worldTransform_.translation_, velocity);

	bullets_.push_back(newBullet);
}


void Player::Draw() {



	model_->Draw(worldTransform_, *camera_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw();
	}

}
