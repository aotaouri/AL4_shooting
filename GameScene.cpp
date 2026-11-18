#include "GameScene.h"
#include "CameraController.h"
#include "Enemy.h"
#include "Item.h"
#include "MyMath.h"
#include "Player.h"
#include "Stage.h"
#include <cmath>

using namespace KamataEngine;

void GameScene::Initialize() {

	// 乱数エンジンの初期化(1回だけ)
	std::random_device seedGen;
	randomEngine_.seed(seedGen());

	// 3Dモデルの生成
	modelPlayer_ = KamataEngine::Model::CreateFromOBJ("player", true);

	// プレイヤーの弾
	modelBullet_ = KamataEngine::Model::CreateFromOBJ("playerBullet", true);

	// 敵
	modelEnemy_ = KamataEngine::Model::CreateFromOBJ("enemy", true);

	// アイテム
	modelItem_ = KamataEngine::Model::CreateFromOBJ("cube", true);

	// スコア UI 用スプライト
	uint32_t textureHandle = KamataEngine::TextureManager::Load("white1x1.png");
	spriteScoreIcon_ = KamataEngine::Sprite::Create(textureHandle, scoreUiPos_);

	spriteScoreIcon_->SetSize({24.0f, 24.0f});
	spriteScoreIcon_->SetColor({1.0f, 0.9f, 0.2f, 1.0f}); // 黄色

	// カメラコントローラを先に作り、カメラを取り出す
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	camera_ = cameraController_->GetCamera();

	// 壁
	modelWall_ = KamataEngine::Model::CreateFromOBJ("cube", true);
	stage_ = new Stage();
	stage_->Initialize(modelWall_, camera_);

	// 自キャラの生成と初期化
	player_ = new Player();
	Vector3 playerPosition = {0.0f, 0.0f, 0.0f};
	player_->Initialize(modelPlayer_, modelBullet_, camera_, playerPosition);

	// 追従対象を設定して、一瞬で追従位置へ合わせる
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
}

void GameScene::Update() {

	// 自キャラの更新
	player_->Update();
	// カメラの追従(プレイヤー更新後に行う)
	cameraController_->Update();

	// View * Projection を1回だけ合成(行ベクトル方式の順序)
	Matrix4x4 viewProjection = Multiply(camera_->matView, camera_->matProjection);

	const Vector3& playerPos = player_->GetWorldTransform().translation_;
	for (Enemy* enemy : enemies_) {
		enemy->Update(playerPos, viewProjection, *stage_);
	}

	// 確認用:Iキーでプレイヤーの横にアイテムを出す
	if (Input::GetInstance()->TriggerKey(DIK_I)) {
		Vector3 pos = playerPos;
		pos.x += 6.0f;
		SpawnItem(pos);
	}

	// アイテムの更新
	for (auto it = items_.begin(); it != items_.end();) {
		Item* item = *it;

		if (item->Update(playerPos, kDeltaTime)) {
			// 取得した瞬間:3D座標を画面座標に変換して2Dスプライトを発生させる
			ScoreSprite sprite;
			sprite.pos = WorldToScreen(item->GetPosition(), viewProjection, (float)WinApp::kWindowWidth, (float)WinApp::kWindowHeight);
			scoreSprites_.push_back(sprite);

			delete item;
			it = items_.erase(it);
		} else {
			++it;
		}
	}

	// 2Dスプライトの更新
	for (auto it = scoreSprites_.begin(); it != scoreSprites_.end();) {
		UpdateScoreSprite(*it);
		if (!it->isActive) {
			it = scoreSprites_.erase(it);
		} else {
			++it;
		}
	}

	spawnTimer_++;
	if (spawnTimer_ >= kSpawnInterval && enemies_.size() < kMaxEnemies) {
		SpawnEnemy(viewProjection);
		spawnTimer_ = 0;
	}
}

void GameScene::SpawnEnemy(const Matrix4x4& viewProjection) {

	const Vector3& playerPos = player_->GetWorldTransform().translation_;

	// 画面外の候補を探す
	auto spawnPos = FindSpawnPosition(playerPos, viewProjection);
	if (!spawnPos.has_value()) {
		return; // 見つからなければ今回は見送る
	}

	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(modelEnemy_, camera_, spawnPos.value());
	enemies_.push_back(newEnemy);
}

std::optional<Vector3> GameScene::FindSpawnPosition(const Vector3& playerPosition, const Matrix4x4& viewProjection) {

	constexpr float pi = 3.1415926535f;
	constexpr float minRadius = 20.0f;
	constexpr float maxRadius = 40.0f;
	constexpr int maxAttempts = 32;

	std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * pi);
	std::uniform_real_distribution<float> radiusDist(minRadius, maxRadius);

	for (int i = 0; i < maxAttempts; ++i) {
		float angle = angleDist(randomEngine_);
		float radius = radiusDist(randomEngine_);

		Vector3 position{playerPosition.x + std::cos(angle) * radius, playerPosition.y, playerPosition.z + std::sin(angle) * radius};

		// 画面内かどうかを反転して、画面外の候補だけ採用する
		if (!IsInsideScreen(position, viewProjection) && !stage_->IsWallAt(position)) {
			return position;
		}
	}
	return std::nullopt;
}

void GameScene::SpawnItem(const Vector3& position) {
	Item* newItem = new Item();
	newItem->Initialize(modelItem_, camera_, position);
	items_.push_back(newItem);
}

void GameScene::UpdateScoreSprite(ScoreSprite& sprite) {

	// 毎フレーム、スコア UI へ近づける(Lerp)
	sprite.pos.x += (scoreUiPos_.x - sprite.pos.x) * kLerpRate;
	sprite.pos.y += (scoreUiPos_.y - sprite.pos.y) * kLerpRate;

	// スコア UI に届いた瞬間にスコアを加算する
	float diffX = scoreUiPos_.x - sprite.pos.x;
	float diffY = scoreUiPos_.y - sprite.pos.y;
	if (std::sqrt(diffX * diffX + diffY * diffY) <= kArriveRange) {
		score_ += kItemScore;
		sprite.isActive = false;
	}
}

void GameScene::Draw() {

	KamataEngine::Model::PreDraw();

	stage_->Draw();

	player_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
	for (Item* item : items_) {
		item->Draw();
	}

	KamataEngine::Model::PostDraw();

	// 2Dスプライトの描画(3Dの後)
	KamataEngine::Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	for (const ScoreSprite& s : scoreSprites_) {
		spriteScoreIcon_->SetPosition(s.pos);
		spriteScoreIcon_->Draw();
	}
	KamataEngine::Sprite::PostDraw();
}

GameScene::~GameScene() {

		delete stage_;
	delete modelWall_;

	delete player_;
	delete cameraController_;
	delete modelPlayer_;
	delete modelBullet_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	delete modelEnemy_;

	for (Item* item : items_) {
		delete item;
	}
	items_.clear();
	delete modelItem_;
	delete spriteScoreIcon_;
}