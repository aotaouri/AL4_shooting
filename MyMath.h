#pragma once
#include "KamataEngine.h"

KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& a, const KamataEngine::Matrix4x4& b);
KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);
KamataEngine::Matrix4x4 MakeRotationMatrix(const KamataEngine::Vector3& rotation);
KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translation);

// WorldTransform の行列を作って転送する
void UpdateWorldTransform(KamataEngine::WorldTransform& worldTransform);

// 行ベクトル方式で Vector3 を Matrix4x4 で変換し、wで割る(NDC座標を返す)
KamataEngine::Vector3 TransformToNDC(const KamataEngine::Vector3& v, const KamataEngine::Matrix4x4& m, float* outW = nullptr);

// ワールド座標の点が画面内にあるか(viewProjection は View * Projection)
bool IsInsideScreen(const KamataEngine::Vector3& worldPos, const KamataEngine::Matrix4x4& viewProjection);

// ワールド座標 → 画面座標(左上が(0,0)のピクセル座標)
KamataEngine::Vector2 WorldToScreen(const KamataEngine::Vector3& worldPos, const KamataEngine::Matrix4x4& viewProjection, float screenWidth, float screenHeight);

bool ScreenToGround(const KamataEngine::Vector2& screenPos, const KamataEngine::Matrix4x4& view, const KamataEngine::Matrix4x4& projection, float screenWidth, float screenHeight, float groundY,KamataEngine::Vector3* out);