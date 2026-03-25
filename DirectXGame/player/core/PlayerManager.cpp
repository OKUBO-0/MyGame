#include "PlayerManager.h"
#include "../../enemy/EnemyManager.h"
#include <fstream>
#include <sstream>

using namespace KamataEngine;

namespace DirectXGame {

void PlayerManager::Initialize(Player* player) {
    player_ = player;

    // 初期値は CSV から読み込むため、ここでは何も設定しない
    if (player_) player_->SetVisible(visible_);
}

void PlayerManager::LoadStatusFromCSV(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        OutputDebugStringA(("Player CSV 読み込み失敗: " + filePath + "\n").c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string key, value;

        std::getline(ss, key, ',');
        std::getline(ss, value, ',');

        if (key == "level") level_ = std::stoi(value);
        else if (key == "nextLevelExp") nextLevelExp_ = std::stoi(value);
        else if (key == "maxLifeStock") maxLifeStock_ = std::stoi(value);
        else if (key == "lifeStock") lifeStock_ = std::stoi(value);
        else if (key == "exp") exp_ = std::stoi(value);
        else if (key == "totalExp") totalExp_ = std::stoi(value);
        else if (key == "attackPower") attackPower_ = std::stoi(value);
    }

    file.close();
}

void PlayerManager::Update() {
    UpdateInvincibility();
    UpdateNormalBullets();
    UpdateOrbitBullets();
    UpdateDrone();
    UpdateEffects();
}

void PlayerManager::Draw(Camera* camera) {
    for (auto& e : effects_) e->Draw(camera);
    for (auto& b : normalBullets_) b->Draw(camera);
    for (auto& orb : orbitBullets_) orb->Draw(camera);
    if (hasDrone_ && drone_) drone_->Draw(camera);
}

void PlayerManager::TakeDamage() {
    if (invincible_) return;

    lifeStock_--;
    invincible_ = true;
    invincibleTimer_ = 1.0f;
    visible_ = false;

    if (player_) player_->SetVisible(false);
}

void PlayerManager::RecoverHP() {
    lifeStock_++;
    if (lifeStock_ > maxLifeStock_) lifeStock_ = maxLifeStock_;
}

void PlayerManager::AddEXP(int32_t amount) {
    exp_ += amount;
    totalExp_ += amount;

    while (exp_ >= nextLevelExp_) {
        exp_ -= nextLevelExp_;
        level_++;
        nextLevelExp_ = static_cast<int32_t>(nextLevelExp_ * 1.5f);
        levelUpRequested_ = true;
    }
}

void PlayerManager::UpdateInvincibility() {
    const float dt = 0.016f;

    if (invincible_) {
        invincibleTimer_ -= dt;
        if (invincibleTimer_ <= 0.0f) {
            invincible_ = false;
            visible_ = true;
            if (player_) player_->SetVisible(true);
        }
        else {
            int blink = static_cast<int>(invincibleTimer_ * 10.0f);
            visible_ = (blink % 2 == 0);
            if (player_) player_->SetVisible(visible_);
        }
    }
}

void PlayerManager::UpdateNormalBullets() {
    const float dt = 0.016f;

    if (hasNormalBullets_ && player_) {
        normalBulletTimer_ += dt;

        if (normalBulletTimer_ >= normalBulletInterval_) {
            float angle = player_->GetWorldRotationY();
            Vector3 forward = { std::sin(angle), 0.0f, std::cos(angle) };

            auto b = std::make_unique<NormalBullet>();
            b->InitializeForward(player_->GetWorldPosition(), forward);
            normalBullets_.push_back(std::move(b));

            normalBulletTimer_ = 0.0f;
        }

        for (auto& b : normalBullets_) b->Update(player_->GetWorldPosition());

        normalBullets_.erase(
            std::remove_if(normalBullets_.begin(), normalBullets_.end(),
                [](const std::unique_ptr<NormalBullet>& b) { return !b->IsActive(); }),
            normalBullets_.end()
        );
    }
}

void PlayerManager::UpdateOrbitBullets() {
    if (hasOrbitBullets_ && player_) {
        for (auto& orb : orbitBullets_) orb->Update(player_->GetWorldPosition());
    }
}

void PlayerManager::UpdateDrone() {
    if (hasDrone_ && drone_) {
        drone_->Update(
            player_->GetWorldPosition(),
            enemyManager_->GetEnemies(),
            droneTimer_,
            droneInterval_
        );
    }
}

void PlayerManager::UpdateEffects() {
    for (auto it = effects_.begin(); it != effects_.end();) {
        (*it)->Update();
        if (!(*it)->IsActive()) it = effects_.erase(it);
        else ++it;
    }
}

void PlayerManager::UpgradeNormalBullets() {
    normalBulletInterval_ *= 0.8f;
}

void PlayerManager::AddOrbitBullets() {
    hasOrbitBullets_ = true;
    orbitBullets_.clear();

    const int count = 1;
    for (int i = 0; i < count; ++i) {
        float angle = (2.0f * 3.14159265f * i) / count;
        auto orb = std::make_unique<OrbitBullet>();
        orb->Initialize(player_->GetWorldPosition(), 10.0f, angle);
        orbitBullets_.push_back(std::move(orb));
    }
}

void PlayerManager::UpgradeOrbitBullets() {
    int newCount = static_cast<int>(orbitBullets_.size()) + 1;

    std::vector<std::unique_ptr<OrbitBullet>> newOrbs;
    newOrbs.reserve(newCount);

    for (int i = 0; i < newCount; ++i) {
        float angle = (2.0f * 3.14159265f * i) / newCount;
        auto orb = std::make_unique<OrbitBullet>();
        orb->Initialize(player_->GetWorldPosition(), 10.0f, angle);
        newOrbs.push_back(std::move(orb));
    }

    orbitBullets_ = std::move(newOrbs);
}

void PlayerManager::AddDrone() {
    hasDrone_ = true;
    drone_ = std::make_unique<Drone>();
    drone_->Initialize({ 3.0f, 2.0f, 0.0f });
}

void PlayerManager::UpgradeDrone() {
    droneInterval_ *= 0.8f;
}

} // namespace DirectXGame
