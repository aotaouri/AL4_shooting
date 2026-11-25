#pragma once
#include "KamataEngine.h"
#include <list>
#include <string>
#include <vector>
#include <random>

class Stage {
public:

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, std::mt19937& randomEngine);

	void Draw();

	// プレイヤーのマスを起点に、全マスへの歩数を求める(プレイヤーが別のマスに移動したときだけ呼ぶ)
	void BuildFlowField(const KamataEngine::Vector3& playerPos);

	// マス座標が壁か(マップの外は壁ではない)
	bool IsWall(int gridX, int gridZ) const;

	// ワールド座標の位置が壁の中か
	bool IsWallAt(const KamataEngine::Vector3& pos) const;

	// startPos から endPos まで壁に遮られずに見通せるか(XZ平面)
	bool HasLineOfSight(const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& endPos) const;

		// 座標がマップの範囲内か
	bool IsInsideMap(const KamataEngine::Vector3& pos) const;

	// 敵の座標から、プレイヤーへ向かう単位ベクトル(XZ)を返す。道が無ければ false
	bool GetFlowDirection(const KamataEngine::Vector3& pos, KamataEngine::Vector3* outDir) const;

	// 1マスの大きさ
	static inline const float kTileSize = 2.0f;
	// 円
	KamataEngine::Vector3 ResolveCollision(const KamataEngine::Vector3& pos, float radius) const;

	// 円(中心 pos、半径 radius)がマップの外へはみ出さないように収めた座標を返す
	KamataEngine::Vector3 ClampToMap(const KamataEngine::Vector3& pos, float radius) const;

	// 床の上面の高さ(壁の下端に合わせる)
	static inline const float kFloorY = -1.0f;

	private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	// マップ(行 = Z方向、列 = X方向)
	std::vector<std::string> map_;
	int width_ = 0;
	int height_ = 0;

	// 壁1マスごとのワールド変換(list なのでアドレスが変わらない)
	std::list<KamataEngine::WorldTransform> wallTransforms_;

	// 床タイルのワールド変換
	std::list<KamataEngine::WorldTransform> floorTransforms_;

	// 巨大な床(1枚)
	KamataEngine::WorldTransform bigFloorTransform_;

	// 床の一辺の半分の長さ
	static inline const float kFloorHalfSize = 100.0f;

		// ランダムなマップを作る
	void GenerateRandomMap(std::mt19937& randomEngine);

	// マップの大きさ(マス数)
	static inline const int kMapWidth = 60;
	static inline const int kMapHeight = 60;

	// 壁になる確率(0.0〜1.0)
	static inline const float kWallRate = 0.10f;

	// スタート地点の周囲で壁を置かないマス数(半径)
	static inline const int kSafeRadius = 3;

	// 距離フィールド(1次元配列で [row * width_ + col])
	std::vector<int> dist_;
	int lastFlowCol_ = -1;
	int lastFlowRow_ = -1;
	static inline const int kInf = 1000000;

};
