#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player;
class CameraController;


class GameScene {

public:

	~GameScene();

	void Initialize();

	void Update();

	void Draw();

	
private:

	// 3Dモデル
	KamataEngine::Model* modelPlayer_ = nullptr;

	// カメラ(所有者は CameraController)
	KamataEngine::Camera* camera_ = nullptr;

	// 自キャラ
	Player* player_ = nullptr;

	// カメラコントローラ
	CameraController* cameraController_ = nullptr;


};
