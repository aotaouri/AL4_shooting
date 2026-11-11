#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player;

class CameraController {

public:
	void Initialize();

	void Update();

	void Reset();

	void SetTarget(Player* target) { target_ = target; }

	// GameScene が描画に使うため、カメラを返す
	KamataEngine::Camera* GetCamera() { return &camera_; }

private:

	// カメラ
	Camera camera_;

	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = {0.0f, 20.0f, -15.0f};

	// カメラの目標座標
	Vector3 destination_ = {};

	// 座標補間割合(1フレームで目標に近づく割合)
	static inline const float kInterpolationRate = 0.1f;

};