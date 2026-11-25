#include "MyMath.h"
#include <cmath>

using namespace KamataEngine;

Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += a.m[i][k] * b.m[k][j];
			}
		}
	}
	return result;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 m = {};
	m.m[0][0] = scale.x;
	m.m[1][1] = scale.y;
	m.m[2][2] = scale.z;
	m.m[3][3] = 1.0f;
	return m;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& t) {
	Matrix4x4 m = {};
	m.m[0][0] = 1.0f;
	m.m[1][1] = 1.0f;
	m.m[2][2] = 1.0f;
	m.m[3][3] = 1.0f;
	m.m[3][0] = t.x;
	m.m[3][1] = t.y;
	m.m[3][2] = t.z;
	return m;
}

Matrix4x4 MakeRotationMatrix(const Vector3& r) {
	Matrix4x4 rx = {};
	rx.m[0][0] = 1.0f;
	rx.m[1][1] = std::cos(r.x);
	rx.m[1][2] = std::sin(r.x);
	rx.m[2][1] = -std::sin(r.x);
	rx.m[2][2] = std::cos(r.x);
	rx.m[3][3] = 1.0f;

	Matrix4x4 ry = {};
	ry.m[0][0] = std::cos(r.y);
	ry.m[0][2] = -std::sin(r.y);
	ry.m[1][1] = 1.0f;
	ry.m[2][0] = std::sin(r.y);
	ry.m[2][2] = std::cos(r.y);
	ry.m[3][3] = 1.0f;

	Matrix4x4 rz = {};
	rz.m[0][0] = std::cos(r.z);
	rz.m[0][1] = std::sin(r.z);
	rz.m[1][0] = -std::sin(r.z);
	rz.m[1][1] = std::cos(r.z);
	rz.m[2][2] = 1.0f;
	rz.m[3][3] = 1.0f;

	return Multiply(Multiply(rx, ry), rz);
}

void UpdateWorldTransform(WorldTransform& worldTransform) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(worldTransform.scale_);
	Matrix4x4 rotationMatrix = MakeRotationMatrix(worldTransform.rotation_);
	Matrix4x4 translationMatrix = MakeTranslateMatrix(worldTransform.translation_);

	// アフィン変換(スケール → 回転 → 平行移動)
	Matrix4x4 worldMatrix = Multiply(scaleMatrix, rotationMatrix);
	worldMatrix = Multiply(worldMatrix, translationMatrix);

	worldTransform.matWorld_ = worldMatrix;
	worldTransform.TransferMatrix();
}

Vector2 WorldToScreen(const Vector3& worldPos, const Matrix4x4& viewProjection, float screenWidth, float screenHeight) {
	// ワールド座標を透視除算済みの NDC へ変換する
	Vector3 ndc = TransformToNDC(worldPos, viewProjection);

	// 画面左上を (0,0) とするピクセル座標へ変換する
	float screenX = (ndc.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - ndc.y) * 0.5f * screenHeight; // Y軸を反転

	return Vector2{screenX, screenY};
}

Vector3 TransformToNDC(const Vector3& v, const Matrix4x4& m, float* outW) {
	// 行ベクトル方式:[x y z 1] * M
	float x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
	float y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
	float z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	if (outW) {
		*outW = w;
	}

	// w で割って NDC にする(w が 0 に近いときは割らない)
	if (std::abs(w) > 0.00001f) {
		x /= w;
		y /= w;
		z /= w;
	}
	return {x, y, z};
}

bool IsInsideScreen(const Vector3& worldPos, const Matrix4x4& viewProjection) {
	float w = 0.0f;
	Vector3 ndc = TransformToNDC(worldPos, viewProjection, &w);

	// カメラの後ろにある点は画面外
	if (w <= 0.0f) {
		return false;
	}

	// NDC は X・Y が -1〜1、Z が 0〜1(DirectX)の範囲なら画面内
	return ndc.x >= -1.0f && ndc.x <= 1.0f && ndc.y >= -1.0f && ndc.y <= 1.0f && ndc.z >= 0.0f && ndc.z <= 1.0f;
}

bool ScreenToGround(const Vector2& screenPos, const Matrix4x4& view, const Matrix4x4& projection, float screenWidth, float screenHeight, float groundY, Vector3* out) {

	// 画面座標 → NDC(WorldToScreen の逆)
	float ndcX = screenPos.x / screenWidth * 2.0f - 1.0f;
	float ndcY = 1.0f - screenPos.y / screenHeight * 2.0f;

	// ビュー空間でのレイの向き(透視投影の前提:z = 1 のときの x, y)
	float vx = ndcX / projection.m[0][0];
	float vy = ndcY / projection.m[1][1];
	float vz = 1.0f;

	// ビュー行列(回転 R と平行移動 t)の逆変換:world = (view空間 - t) * R^T
	auto toWorld = [&](float x, float y, float z) {
		x -= view.m[3][0];
		y -= view.m[3][1];
		z -= view.m[3][2];
		return Vector3{x * view.m[0][0] + y * view.m[0][1] + z * view.m[0][2], x * view.m[1][0] + y * view.m[1][1] + z * view.m[1][2], x * view.m[2][0] + y * view.m[2][1] + z * view.m[2][2]};
	};

	// レイの始点(カメラ位置)と、向きの先の点
	Vector3 origin = toWorld(0.0f, 0.0f, 0.0f);
	Vector3 pointOnRay = toWorld(vx, vy, vz);
	Vector3 dir = {pointOnRay.x - origin.x, pointOnRay.y - origin.y, pointOnRay.z - origin.z};

	// 地面に向かっていないレイは交点なし
	if (std::abs(dir.y) < 0.00001f) {
		return false;
	}
	float t = (groundY - origin.y) / dir.y;
	if (t <= 0.0f) {
		return false;
	}

	*out = {origin.x + dir.x * t, groundY, origin.z + dir.z * t};
	return true;
}