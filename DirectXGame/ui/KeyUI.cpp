#include "KeyUI.h"
using namespace KamataEngine;

void KeyUI::Initialize() {

	keyW_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("key_w.png"), {0, 0}));
	keyA_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("key_a.png"), {0, 0}));
	keyS_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("key_s.png"), {0, 0}));
	keyD_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("key_d.png"), {0, 0}));

	// ESC 通常時
	keyESC_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("key_esc.png"), {0, 0}));

	// 全て同じサイズで表示
	keyW_->SetSize({1280, 720});
	keyA_->SetSize({1280, 720});
	keyS_->SetSize({1280, 720});
	keyD_->SetSize({1280, 720});

	keyESC_->SetSize({1280, 720});
}

void KeyUI::Update(Input* input) {
	SetKeyColor(keyW_.get(), input->PushKey(DIK_W));
	SetKeyColor(keyA_.get(), input->PushKey(DIK_A));
	SetKeyColor(keyS_.get(), input->PushKey(DIK_S));
	SetKeyColor(keyD_.get(), input->PushKey(DIK_D));
}

void KeyUI::SetKeyColor(Sprite* key, bool pressed) {
	if (pressed)
		key->SetColor({1, 1, 0, 1});
	else
		key->SetColor({1, 1, 1, 1});
}

void KeyUI::Draw() {
	keyW_->Draw();
	keyA_->Draw();
	keyS_->Draw();
	keyD_->Draw();
	keyESC_->Draw();
}