#include "KeyUI.h"
#include "../../core/InputBindings.h"
using namespace KamataEngine;

namespace DirectXGame {

void KeyUI::Initialize() {
	keyW_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/controls/key_W.png"), {0, 0}));
	keyA_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/controls/key_a.png"), {0, 0}));
	keyS_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/controls/key_s.png"), {0, 0}));
	keyD_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/controls/key_d.png"), {0, 0}));

	// ESC 通常時
	keyESC_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/controls/key_esc.png"), {0, 0}));

	// 全て同じサイズで表示
	keyW_->SetSize({1280, 720});
	keyA_->SetSize({1280, 720});
	keyS_->SetSize({1280, 720});
	keyD_->SetSize({1280, 720});

	keyESC_->SetSize({1280, 720});
}

void KeyUI::Update(Input* input) {
	const Vector2 move = InputBindings::GetMoveVector(input);
	SetKeyColor(keyW_.get(), move.y > 0.15f);
	SetKeyColor(keyA_.get(), move.x < -0.15f);
	SetKeyColor(keyS_.get(), move.y < -0.15f);
	SetKeyColor(keyD_.get(), move.x > 0.15f);
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

} // namespace DirectXGame
