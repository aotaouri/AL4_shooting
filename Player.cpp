#include "Player.h"
#include "MyMath.h"
#include "PlayerBullet.h"
#include "Stage.h"
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
	}

	// 速度を求めて座標に加算
	velocity_.x = direction.x * kMoveSpeed;
	velocity_.y = 0.0f;
	velocity_.z = direction.z * kMoveSpeed;

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.z += velocity_.z;

	// 左クリックで弾を撃つ
	if (input->IsTriggerMouse(0)) {
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

//プレイヤーの弾の当たり判定
bool Player::HitBullet(const Vector3& enemyPos, float enemyRadius) {

	// 弾の半径(モデルの大きさに合わせて調整)
	constexpr float kBulletRadius = 0.3f;
	const float hitDist = enemyRadius + kBulletRadius;

	for (auto it = bullets_.begin(); it != bullets_.end(); ++it) {
		const Vector3& bp = (*it)->GetPosition();
		float diffX = bp.x - enemyPos.x;
		float diffZ = bp.z - enemyPos.z;

		if (diffX * diffX + diffZ * diffZ <= hitDist * hitDist) {
			delete *it;
			bullets_.erase(it); // 1発の弾は1体まで
			return true;
		}
	}
	return false;
}

void Player::RefreshTransform() {
	UpdateWorldTransform(worldTransform_); 
}

void Player::RemoveBulletsHitWall(const Stage& stage) {

	bullets_.remove_if([&stage](PlayerBullet* bullet) {
		if (stage.IsWallAt(bullet->GetPosition())) {
			delete bullet;
			return true;
		}
		return false;
	});
}

void Player::LookAt(const Vector3& target) {

	float diffX = target.x - worldTransform_.translation_.x;
	float diffZ = target.z - worldTransform_.translation_.z;
	float length = std::sqrt(diffX * diffX + diffZ * diffZ);

	// マウスがプレイヤーの真上にあるときは向きを変えない
	if (length < 0.001f) {
		return;
	}

	aimDirection_ = {diffX / length, 0.0f, diffZ / length};
	worldTransform_.rotation_.y = std::atan2(diffX, diffZ);
}

void Player::Draw() {



	model_->Draw(worldTransform_, *camera_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw();
	}

}
