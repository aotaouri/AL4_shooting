#pragma once
#include "KamataEngine.h"
#include <list>
#include <optional>
#include <random>

using namespace KamataEngine;

class Player;
class Enemy;
class CameraController;
class Item;
class Stage; 

// 2D演出用スプライト
struct ScoreSprite {
	Vector2 pos;
	bool isActive = true;
};

class GameScene {
public:
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

private:
	// 敵をスポーンさせる
	void SpawnEnemy(const Matrix4x4& viewProjection);

	void SpawnItem(const Vector3& position);

	// スコアスプライトの更新(届いたらスコア加算)
	void UpdateScoreSprite(ScoreSprite& sprite);

	// 敵同士の重なりを解消する
	void SeparateEnemies();

	// 画面外のスポーン位置を探す
	std::optional<Vector3> FindSpawnPosition(const Vector3& playerPosition, const Matrix4x4& viewProjection);

	// 3Dモデル
	KamataEngine::Model* modelPlayer_ = nullptr;

	// 弾のモデル
	KamataEngine::Model* modelBullet_ = nullptr;
	// 敵
	KamataEngine::Model* modelEnemy_ = nullptr;
	std::list<Enemy*> enemies_;

	// アイテム
	KamataEngine::Model* modelItem_ = nullptr;
	std::list<Item*> items_;

	KamataEngine::Model* modelWall_ = nullptr;
	Stage* stage_ = nullptr;

	// 2D演出
	KamataEngine::Sprite* spriteScoreIcon_ = nullptr;
	std::list<ScoreSprite> scoreSprites_;

	// スコア UI の座標
	Vector2 scoreUiPos_ = {60.0f, 40.0f};
	int score_ = 0;

	// カメラ(所有者は CameraController)
	KamataEngine::Camera* camera_ = nullptr;
	// 自キャラ
	Player* player_ = nullptr;
	// カメラコントローラ
	CameraController* cameraController_ = nullptr;

	// 乱数エンジン
	std::mt19937 randomEngine_;
	// スポーンまでのカウンタ
	int spawnTimer_ = 0;
	// スポーン間隔
	static inline const int kSpawnInterval = 40;
	// 同時に存在できる敵の最大数
	static inline const size_t kMaxEnemies = 40;

	// 取得した数(2D演出やスコアに使う想定)
	int collectedCount_ = 0;

	// 1フレームの時間(60fps固定)
	static inline const float kDeltaTime = 1.0f / 60.0f;
	static inline const float kLerpRate = 0.12f;
	static inline const float kArriveRange = 8.0f;
	static inline const int kItemScore = 100;

};
