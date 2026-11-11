#pragma once
#include "KamataEngine.h"

KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& a, const KamataEngine::Matrix4x4& b);
KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);
KamataEngine::Matrix4x4 MakeRotationMatrix(const KamataEngine::Vector3& rotation);
KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translation);

// WorldTransform の行列を作って転送する
void UpdateWorldTransform(KamataEngine::WorldTransform& worldTransform);
