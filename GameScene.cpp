#include "GameScene.h"
#include "CameraController.h"
#include "Player.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// 3Dモデルの生成
	modelPlayer_ = KamataEngine::Model::CreateFromOBJ("player", true);

	// カメラコントローラを先に作り、カメラを取り出す
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	camera_ = cameraController_->GetCamera();

	// 自キャラの生成と初期化
	player_ = new Player();
	Vector3 playerPosition = {0.0f, 0.0f, 0.0f};
	player_->Initialize(modelPlayer_, camera_, playerPosition);

	// 追従対象を設定して、一瞬で追従位置へ合わせる
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

}

void GameScene::Update() {

	// 自キャラの更新
	player_->Update();

	// カメラの追従(プレイヤー更新後に行う)
	cameraController_->Update();

}

void GameScene::Draw() {


	KamataEngine::Model::PreDraw();

	player_->Draw();

	KamataEngine::Model::PostDraw();

}

GameScene::~GameScene() {

	delete player_;
	delete cameraController_; // camera_ は CameraController が持つので delete しない
	delete modelPlayer_;

}