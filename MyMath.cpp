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