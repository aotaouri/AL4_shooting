#include "Stage.h"
#include "MyMath.h"
#include <cmath>

using namespace KamataEngine;

namespace {
// '#' が壁、'.' が空き。20x20 マスで、中心(列10・行10)がワールド原点
const char* kMapData[] = {
    "....................", // 0
    "....................", // 1
    "....#####...........", // 2
    "....................", // 3
    "....................", // 4
    "..............#.....", // 5
    "..............#.....", // 6
    "..............#.....", // 7
    "....................", // 8
    "....................", // 9
    "....................", // 10
    "....................", // 11
    "..###...............", // 12
    "....................", // 13
    "....................", // 14
    ".........#..........", // 15
    ".........#..........", // 16
    "....................", // 17
    "....................", // 18
    "....................", // 19
};
} // namespace

void Stage::Initialize(Model* model, Camera* camera) {

	model_ = model;
	camera_ = camera;

	for (const char* row : kMapData) {
		map_.push_back(row);
	}
	height_ = static_cast<int>(map_.size());
	width_ = static_cast<int>(map_[0].size());

	// 壁のマスごとにワールド変換を作る
	for (int r = 0; r < height_; ++r) {
		for (int c = 0; c < width_; ++c) {
			if (map_[r][c] != '#') {
				continue;
			}
			wallTransforms_.emplace_back();
			WorldTransform& wt = wallTransforms_.back();
			wt.Initialize();

			// マスの中心に置く
			wt.translation_ = {(c - width_ / 2 + 0.5f) * kTileSize, 0.0f, (r - height_ / 2 + 0.5f) * kTileSize};
			// cube.obj が 2x2x2 の前提で、1マスの大きさに合わせる
			wt.scale_ = {kTileSize / 2.0f, kTileSize / 2.0f, kTileSize / 2.0f};

			UpdateWorldTransform(wt);
		}
	}
}

void Stage::Draw() {
	for (WorldTransform& wt : wallTransforms_) {
		model_->Draw(wt, *camera_);
	}
}

bool Stage::IsWall(int gridX, int gridZ) const {

	// マス座標(負の値あり)を配列の添字に直す
	int col = gridX + width_ / 2;
	int row = gridZ + height_ / 2;

	if (col < 0 || col >= width_ || row < 0 || row >= height_) {
		return false;
	}
	return map_[row][col] == '#';
}

bool Stage::IsWallAt(const Vector3& pos) const {
	int gridX = static_cast<int>(std::floor(pos.x / kTileSize));
	int gridZ = static_cast<int>(std::floor(pos.z / kTileSize));
	return IsWall(gridX, gridZ);
}

bool Stage::HasLineOfSight(const Vector3& startPos, const Vector3& endPos) const {

	// 座標を tileSize で割って、マス単位の座標にする
	float startX = startPos.x / kTileSize;
	float startZ = startPos.z / kTileSize;
	float endX = endPos.x / kTileSize;
	float endZ = endPos.z / kTileSize;

	// 始点マスと終点マス
	int gridX = static_cast<int>(std::floor(startX));
	int gridZ = static_cast<int>(std::floor(startZ));
	int endGridX = static_cast<int>(std::floor(endX));
	int endGridZ = static_cast<int>(std::floor(endZ));

	float dx = endX - startX;
	float dz = endZ - startZ;
	int stepX = (dx >= 0.0f) ? 1 : -1;
	int stepZ = (dz >= 0.0f) ? 1 : -1;

	// 1マス分の間隔 tDelta(0 除算の回避に大きな値を入れる)
	constexpr float kInf = 1e30f;
	float tDeltaX = (dx != 0.0f) ? std::abs(1.0f / dx) : kInf;
	float tDeltaZ = (dz != 0.0f) ? std::abs(1.0f / dz) : kInf;

	// 最初の境界までの t
	float nextX = static_cast<float>(dx >= 0.0f ? gridX + 1 : gridX);
	float nextZ = static_cast<float>(dz >= 0.0f ? gridZ + 1 : gridZ);
	float tMaxX = std::abs(nextX - startX) * tDeltaX;
	float tMaxZ = std::abs(nextZ - startZ) * tDeltaZ;

	// 浮動小数点の誤差で終点に着かなくても、無限ループにならない上限
	int maxSteps = std::abs(endGridX - gridX) + std::abs(endGridZ - gridZ) + 2;

	// 終点マスに着くまで、tMax の小さい方へ1マスずつ進む
	while ((gridX != endGridX || gridZ != endGridZ) && maxSteps-- > 0) {
		if (tMaxX < tMaxZ) {
			gridX += stepX;
			tMaxX += tDeltaX;
		} else {
			gridZ += stepZ;
			tMaxZ += tDeltaZ;
		}

		if (IsWall(gridX, gridZ)) {
			return false; // 壁に遮られた
		}
	}
	return true; // 終点マスまで見通せた
}