#include "Stage.h"
#include "MyMath.h"
#include <cmath>
#include <queue>

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

void Stage::Initialize(Model* model, Camera* camera, std::mt19937& randomEngine) {

	model_ = model;
	camera_ = camera;

	// 固定マップの代わりに、ランダムなマップを作る
	GenerateRandomMap(randomEngine);

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

		// 巨大な床を1枚だけ敷く(cube.obj が 2x2x2 の前提)
	bigFloorTransform_.Initialize();
	bigFloorTransform_.translation_ = {0.0f, kFloorY - 0.1f, 0.0f};
	bigFloorTransform_.scale_ = {kFloorHalfSize / 1.0f, 0.1f, kFloorHalfSize / 1.0f};
	UpdateWorldTransform(bigFloorTransform_);

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
	constexpr float kHuge = 1e30f;
	float tDeltaX = (dx != 0.0f) ? std::abs(1.0f / dx) : kHuge;
	float tDeltaZ = (dz != 0.0f) ? std::abs(1.0f / dz) : kHuge;

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

Vector3 Stage::ResolveCollision(const Vector3& pos, float radius) const {

	Vector3 result = pos;

	// 円が重なりうるマスの範囲
	int minX = static_cast<int>(std::floor((pos.x - radius) / kTileSize));
	int maxX = static_cast<int>(std::floor((pos.x + radius) / kTileSize));
	int minZ = static_cast<int>(std::floor((pos.z - radius) / kTileSize));
	int maxZ = static_cast<int>(std::floor((pos.z + radius) / kTileSize));

	for (int gz = minZ; gz <= maxZ; ++gz) {
		for (int gx = minX; gx <= maxX; ++gx) {
			if (!IsWall(gx, gz)) {
				continue;
			}

			// このマスの範囲(ワールド座標)
			float left = gx * kTileSize;
			float right = (gx + 1) * kTileSize;
			float bottom = gz * kTileSize;
			float top = (gz + 1) * kTileSize;

			// マス上で、円の中心に最も近い点
			float nearestX = std::fmax(left, std::fmin(result.x, right));
			float nearestZ = std::fmax(bottom, std::fmin(result.z, top));

			float diffX = result.x - nearestX;
			float diffZ = result.z - nearestZ;
			float distSq = diffX * diffX + diffZ * diffZ;

			if (distSq >= radius * radius) {
				continue; // 重なっていない
			}

			if (distSq > 0.00001f) {
				// 中心がマスの外側:最も近い点から離れる向きに、めり込んだ分だけ押し出す
				float dist = std::sqrt(distSq);
				float push = radius - dist;
				result.x += diffX / dist * push;
				result.z += diffZ / dist * push;
			} else {
				// 中心がマスの内側(速度が速すぎて入り込んだ場合など):最も近い辺から押し出す
				float toLeft = result.x - left;
				float toRight = right - result.x;
				float toBottom = result.z - bottom;
				float toTop = top - result.z;
				float minDist = std::fmin(std::fmin(toLeft, toRight), std::fmin(toBottom, toTop));

				if (minDist == toLeft) {
					result.x = left - radius;
				} else if (minDist == toRight) {
					result.x = right + radius;
				} else if (minDist == toBottom) {
					result.z = bottom - radius;
				} else {
					result.z = top + radius;
				}
			}
		}
	}
	return result;
}

Vector3 Stage::ClampToMap(const Vector3& pos, float radius) const {

	float halfW = width_ * kTileSize / 2.0f - radius;
	float halfH = height_ * kTileSize / 2.0f - radius;

	Vector3 result = pos;
	result.x = std::fmax(-halfW, std::fmin(result.x, halfW));
	result.z = std::fmax(-halfH, std::fmin(result.z, halfH));
	return result;
}

void Stage::GenerateRandomMap(std::mt19937& randomEngine) {

	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	map_.clear();
	for (int r = 0; r < kMapHeight; ++r) {
		std::string row(kMapWidth, '.');

		for (int c = 0; c < kMapWidth; ++c) {
			// 原点(プレイヤーの開始位置)のマス。IsWall と同じ式で座標を対応させる
			int gridX = c - kMapWidth / 2;
			int gridZ = r - kMapHeight / 2;

			// スタート地点の周囲は壁を置かない
			if (std::abs(gridX) <= kSafeRadius && std::abs(gridZ) <= kSafeRadius) {
				continue;
			}

			if (dist(randomEngine) < kWallRate) {
				row[c] = '#';
			}
		}
		map_.push_back(row);
	}
}

bool Stage::IsInsideMap(const Vector3& pos) const {
	float halfW = width_ * kTileSize / 2.0f;
	float halfH = height_ * kTileSize / 2.0f;
	return pos.x >= -halfW && pos.x < halfW && pos.z >= -halfH && pos.z < halfH;
}

void Stage::BuildFlowField(const Vector3& playerPos) {

	// プレイヤーのマス(配列の添字)
	int startCol = static_cast<int>(std::floor(playerPos.x / kTileSize)) + width_ / 2;
	int startRow = static_cast<int>(std::floor(playerPos.z / kTileSize)) + height_ / 2;

	// マップの外にいるときは更新しない
	if (startCol < 0 || startCol >= width_ || startRow < 0 || startRow >= height_) {
		return;
	}
	// 同じマスにいる間は再計算しない
	if (startCol == lastFlowCol_ && startRow == lastFlowRow_) {
		return;
	}
	lastFlowCol_ = startCol;
	lastFlowRow_ = startRow;

	// 全マスを未到達で初期化してから、始点を 0 にする
	dist_.assign(width_ * height_, kInf);
	std::queue<std::pair<int, int>> q;
	dist_[startRow * width_ + startCol] = 0;
	q.push({startCol, startRow});

	constexpr int dx[4] = {1, -1, 0, 0};
	constexpr int dz[4] = {0, 0, 1, -1};

	while (!q.empty()) {
		auto [cx, cz] = q.front();
		q.pop();
		for (int i = 0; i < 4; ++i) {
			int nx = cx + dx[i];
			int nz = cz + dz[i];
			if (nx < 0 || nx >= width_ || nz < 0 || nz >= height_) {
				continue;
			}
			if (map_[nz][nx] == '#') {
				continue;
			}
			if (dist_[nz * width_ + nx] == kInf) {
				dist_[nz * width_ + nx] = dist_[cz * width_ + cx] + 1;
				q.push({nx, nz});
			}
		}
	}
}

bool Stage::GetFlowDirection(const Vector3& pos, Vector3* outDir) const {

	if (dist_.empty()) {
		return false;
	}

	int x = static_cast<int>(std::floor(pos.x / kTileSize)) + width_ / 2;
	int z = static_cast<int>(std::floor(pos.z / kTileSize)) + height_ / 2;
	if (x < 0 || x >= width_ || z < 0 || z >= height_) {
		return false;
	}

	int c = dist_[z * width_ + x];
	if (c == kInf) {
		return false; // 壁の中、またはプレイヤーに到達できないマス
	}

	// 隣のマスの距離(壁・未到達・範囲外は自分の距離 c として扱う)
	auto at = [&](int ax, int az) {
		if (ax < 0 || ax >= width_ || az < 0 || az >= height_) {
			return c;
		}
		int v = dist_[az * width_ + ax];
		return (v == kInf) ? c : v;
	};

	// 距離が減る向き(左右・前後の差)
	float dirX = static_cast<float>(at(x - 1, z) - at(x + 1, z));
	float dirZ = static_cast<float>(at(x, z - 1) - at(x, z + 1));

	float len = std::sqrt(dirX * dirX + dirZ * dirZ);
	if (len < 0.0001f) {
		return false;
	}
	*outDir = {dirX / len, 0.0f, dirZ / len};
	return true;
}

void Stage::Draw() {
	model_->Draw(bigFloorTransform_, *camera_);
	for (WorldTransform& wt : wallTransforms_) {
		model_->Draw(wt, *camera_);
	}
}