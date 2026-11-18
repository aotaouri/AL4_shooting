#pragma once
#include "KamataEngine.h"
#include <list>
#include <string>
#include <vector>

class Stage {
public:

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);

	void Draw();

	// マス座標が壁か(マップの外は壁ではない)
	bool IsWall(int gridX, int gridZ) const;

	// ワールド座標の位置が壁の中か
	bool IsWallAt(const KamataEngine::Vector3& pos) const;

	// startPos から endPos まで壁に遮られずに見通せるか(XZ平面)
	bool HasLineOfSight(const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& endPos) const;

	// 1マスの大きさ
	static inline const float kTileSize = 2.0f;

	private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	// マップ(行 = Z方向、列 = X方向)
	std::vector<std::string> map_;
	int width_ = 0;
	int height_ = 0;

	// 壁1マスごとのワールド変換(list なのでアドレスが変わらない)
	std::list<KamataEngine::WorldTransform> wallTransforms_;

};
