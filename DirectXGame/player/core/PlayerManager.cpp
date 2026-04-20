#include "PlayerManager.h"
#include "../../enemy/EnemyManager.h"
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace KamataEngine;

namespace DirectXGame {

void PlayerManager::Initialize(Player* player) {
    player_ = player;
    LoadWeaponUpgradeSettings("Resources/data/weaponUpgradeSettings.csv");

    // 初期値は CSV から読み込むため、ここでは何も設定しない
    if (player_) player_->SetVisible(visible_);
    if (player_) {
        previousEffectPosition_ = player_->GetWorldPosition();
        hasPreviousEffectPosition_ = true;
    }
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
        else if (key == "invincibilityDuration") invincibilityDuration_ = std::stof(value);
        else if (key == "normalBulletInterval") normalBulletInterval_ = std::stof(value);
        else if (key == "normalBulletUpgradeMultiplier") normalBulletUpgradeMultiplier_ = std::stof(value);
        else if (key == "normalBulletMinInterval") normalBulletMinInterval_ = std::stof(value);
        else if (key == "droneInterval") droneInterval_ = std::stof(value);
        else if (key == "droneUpgradeMultiplier") droneUpgradeMultiplier_ = std::stof(value);
        else if (key == "maxLifeStockCap") maxLifeStockCap_ = std::stoi(value);
        else if (key == "moveSpeedUpgradeCap") moveSpeedUpgradeCap_ = std::stoi(value);
        else if (key == "moveSpeedUpgradeStep") moveSpeedUpgradeStep_ = std::stof(value);
        else if (key == "moveSpeedMax") moveSpeedMax_ = std::stof(value);
    }

    file.close();
}

void PlayerManager::LoadWeaponUpgradeSettings(const std::string& filePath) {
    weaponUpgradeSettings_.clear();

    std::ifstream file(filePath);
    if (!file.is_open()) {
        OutputDebugStringA(("Weapon upgrade settings 読み込み失敗: " + filePath + "\n").c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string key;
        std::string value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        if (key.empty() || value.empty()) {
            continue;
        }

        weaponUpgradeSettings_[key] = std::stof(value);
    }
}

void PlayerManager::Update(float deltaTime) {
    UpdateInvincibility(deltaTime);
    UpdateNormalBullets(deltaTime);
    UpdateOrbitBullets(deltaTime);
    UpdateDrone(deltaTime);
    UpdateLightning(deltaTime);
    UpdateEffects(deltaTime);
}

void PlayerManager::Draw(Camera* camera) {
    for (auto& e : effects_) e->Draw(camera);
    for (auto& effect : lightningEffects_) effect->Draw(camera);
    for (auto& b : normalBullets_) b->Draw(camera);
    for (auto& orb : orbitBullets_) orb->Draw(camera);
    if (hasDrone_ && drone_) drone_->Draw(camera);
}

void PlayerManager::TakeDamage() {
    if (invincible_) return;

    lifeStock_--;
    invincible_ = true;
    invincibleTimer_ = invincibilityDuration_;
    visible_ = false;

    if (player_) player_->SetVisible(false);
}

void PlayerManager::RecoverHP() {
    lifeStock_++;
    if (lifeStock_ > maxLifeStock_) lifeStock_ = maxLifeStock_;
}

void PlayerManager::IncreaseMaxHP() {
    if (maxLifeStock_ >= maxLifeStockCap_) {
        RecoverHP();
        return;
    }

    ++maxLifeStock_;
    lifeStock_ = maxLifeStock_;
}

void PlayerManager::UpgradeMoveSpeed() {
    if (!player_) {
        return;
    }

    if (moveSpeedLevel_ >= moveSpeedUpgradeCap_) {
        UpgradeAttackPower();
        return;
    }

    const float upgradedSpeed = (std::min)(moveSpeedMax_, player_->GetMoveSpeed() + moveSpeedUpgradeStep_);
    player_->SetMoveSpeed(upgradedSpeed);
    ++moveSpeedLevel_;
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

void PlayerManager::UpdateInvincibility(float deltaTime) {
    if (invincible_) {
        invincibleTimer_ -= deltaTime;
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

void PlayerManager::UpdateNormalBullets(float deltaTime) {
    if (hasNormalBullets_ && player_) {
        normalBulletTimer_ += deltaTime;

        while (normalBulletTimer_ >= normalBulletInterval_) {
            const float angle = player_->GetWorldRotationY();
            const Vector3 forward = { std::sin(angle), 0.0f, std::cos(angle) };
            const Vector3 right = { forward.z, 0.0f, -forward.x };
            const float centerOffset = static_cast<float>(normalBulletAmount_ - 1) * 0.5f;

            for (int32_t i = 0; i < normalBulletAmount_; ++i) {
                Vector3 startPosition = player_->GetWorldPosition();
                const float horizontalOffset = (static_cast<float>(i) - centerOffset) * 1.25f;
                startPosition.x += right.x * horizontalOffset;
                startPosition.z += right.z * horizontalOffset;

                auto b = std::make_unique<NormalBullet>();
                b->InitializeForward(startPosition, forward, normalBulletSpeed_, normalBulletRange_, normalBulletPierceCount_);
                normalBullets_.push_back(std::move(b));
            }

            normalBulletTimer_ -= normalBulletInterval_;
        }

        for (auto& b : normalBullets_) b->Update(player_->GetWorldPosition(), deltaTime);

        normalBullets_.erase(
            std::remove_if(normalBullets_.begin(), normalBullets_.end(),
                [](const std::unique_ptr<NormalBullet>& b) { return !b->IsActive(); }),
            normalBullets_.end()
        );
    }
}

void PlayerManager::UpdateOrbitBullets(float deltaTime) {
    if (hasOrbitBullets_ && player_) {
        for (auto& orb : orbitBullets_) orb->Update(player_->GetWorldPosition(), deltaTime);
    }
}

void PlayerManager::UpdateDrone(float deltaTime) {
    if (hasDrone_ && drone_ && player_ && enemyManager_) {
        drone_->Update(
            player_->GetWorldPosition(),
            enemyManager_->GetEnemies(),
            droneTimer_,
            droneInterval_,
            droneShotCount_,
            droneBulletSpeed_,
            droneBulletRange_,
            dronePierceCount_,
            deltaTime
        );
    }
}

void PlayerManager::UpdateEffects(float deltaTime) {
    if (player_) {
        const Vector3 currentPosition = player_->GetWorldPosition();
        if (!hasPreviousEffectPosition_) {
            previousEffectPosition_ = currentPosition;
            hasPreviousEffectPosition_ = true;
        }

        const float dx = currentPosition.x - previousEffectPosition_.x;
        const float dz = currentPosition.z - previousEffectPosition_.z;
        const float movedDistanceSq = dx * dx + dz * dz;
        constexpr float kMinMoveDistanceSq = 0.01f;
        if (movedDistanceSq > kMinMoveDistanceSq) {
            effectTimer_ += deltaTime;
            while (effectTimer_ >= kEffectInterval) {
                SpawnRippleEffect(currentPosition);
                effectTimer_ -= kEffectInterval;
            }
        } else {
            effectTimer_ = 0.0f;
        }

        previousEffectPosition_ = currentPosition;
    }

    for (auto it = effects_.begin(); it != effects_.end();) {
        (*it)->Update(deltaTime);
        if (!(*it)->IsActive()) it = effects_.erase(it);
        else ++it;
    }

    for (auto it = lightningEffects_.begin(); it != lightningEffects_.end();) {
        (*it)->Update(deltaTime);
        if (!(*it)->IsActive()) {
            it = lightningEffects_.erase(it);
        } else {
            ++it;
        }
    }
}

void PlayerManager::SpawnRippleEffect(const Vector3& position) {
    auto effect = std::make_unique<RippleEffect>();
    effect->Initialize(position);
    effects_.push_back(std::move(effect));
}

void PlayerManager::PlayLevelUpEffect() {
    if (!player_) {
        return;
    }

    const Vector3 center = player_->GetWorldPosition();
    SpawnRippleEffect(center);
    SpawnRippleEffect({ center.x + 2.5f, center.y, center.z });
    SpawnRippleEffect({ center.x - 2.5f, center.y, center.z });
    SpawnRippleEffect({ center.x, center.y, center.z + 2.5f });
    SpawnRippleEffect({ center.x, center.y, center.z - 2.5f });
}

float PlayerManager::GetWeaponUpgradeSetting(const std::string& key, float fallback) const {
    if (const auto it = weaponUpgradeSettings_.find(key); it != weaponUpgradeSettings_.end()) {
        return it->second;
    }
    return fallback;
}

void PlayerManager::UpgradeNormalBullets() {
    if (normalBulletLevel_ >= kNormalBulletMaxLevel) {
        return;
    }

    ++normalBulletLevel_;
    switch (normalBulletLevel_) {
    case 2:
        normalBulletAmount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("normal.lv2.amount", 2.0f));
        break;
    case 3:
        normalBulletSpeed_ *= GetWeaponUpgradeSetting("normal.lv3.speedMultiplier", 1.2f);
        normalBulletInterval_ *= GetWeaponUpgradeSetting("normal.lv3.intervalMultiplier", 0.92f);
        break;
    case 4:
        normalBulletAmount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("normal.lv4.amount", 3.0f));
        break;
    case 5:
        normalBulletDamageBonus_ += static_cast<int32_t>(GetWeaponUpgradeSetting("normal.lv5.damageBonusAdd", 1.0f));
        break;
    case 6:
        normalBulletAmount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("normal.lv6.amount", 4.0f));
        break;
    case 7:
        normalBulletPierceCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("normal.lv7.pierceCount", 2.0f));
        break;
    case 8:
        normalBulletDamageBonus_ += static_cast<int32_t>(GetWeaponUpgradeSetting("normal.lv8.damageBonusAdd", 1.0f));
        normalBulletSpeed_ *= GetWeaponUpgradeSetting("normal.lv8.speedMultiplier", 1.15f);
        normalBulletInterval_ *= GetWeaponUpgradeSetting("normal.lv8.intervalMultiplier", 0.88f);
        break;
    default:
        break;
    }

    normalBulletInterval_ = (std::max)(normalBulletMinInterval_, normalBulletInterval_);
}

void PlayerManager::AddOrbitBullets() {
    hasOrbitBullets_ = true;
    orbitBulletLevel_ = 1;
    orbitBulletCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("orbit.lv1.count", 1.0f));
    orbitBulletScale_ = GetWeaponUpgradeSetting("orbit.lv1.scale", 1.0f);
    orbitHitInterval_ = GetWeaponUpgradeSetting("orbit.lv1.hitInterval", 0.5f);
    RebuildOrbitBullets();
}

void PlayerManager::UpgradeOrbitBullets() {
    if (!hasOrbitBullets_) {
        AddOrbitBullets();
        return;
    }

    if (orbitBulletLevel_ >= kOrbitBulletMaxLevel) {
        return;
    }

    ++orbitBulletLevel_;
    switch (orbitBulletLevel_) {
    case 2:
        orbitBulletCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("orbit.lv2.count", 2.0f));
        break;
    case 3:
        orbitRadius_ += GetWeaponUpgradeSetting("orbit.lv3.radiusAdd", orbitRadiusUpgradeStep_);
        orbitAngularSpeed_ += GetWeaponUpgradeSetting("orbit.lv3.angularSpeedAdd", orbitAngularSpeedUpgradeStep_);
        orbitBulletScale_ += GetWeaponUpgradeSetting("orbit.lv3.scaleAdd", orbitBulletScaleUpgradeStep_);
        break;
    case 4:
        orbitHitInterval_ *= GetWeaponUpgradeSetting("orbit.lv4.hitIntervalMultiplier", orbitHitIntervalUpgradeMultiplier_);
        break;
    case 5:
        orbitBulletCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("orbit.lv5.count", 3.0f));
        break;
    case 6:
        orbitRadius_ += GetWeaponUpgradeSetting("orbit.lv6.radiusAdd", orbitRadiusUpgradeStep_);
        orbitAngularSpeed_ += GetWeaponUpgradeSetting("orbit.lv6.angularSpeedAdd", orbitAngularSpeedUpgradeStep_);
        orbitBulletScale_ += GetWeaponUpgradeSetting("orbit.lv6.scaleAdd", orbitBulletScaleUpgradeStep_);
        break;
    case 7:
        orbitHitInterval_ *= GetWeaponUpgradeSetting("orbit.lv7.hitIntervalMultiplier", orbitHitIntervalUpgradeMultiplier_);
        break;
    case 8:
        orbitBulletCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("orbit.lv8.count", 4.0f));
        break;
    default:
        break;
    }

    RebuildOrbitBullets();
}

void PlayerManager::RebuildOrbitBullets() {
    if (!player_) {
        orbitBullets_.clear();
        return;
    }

    orbitBullets_.clear();
    orbitBullets_.reserve(orbitBulletCount_);

    for (int32_t i = 0; i < orbitBulletCount_; ++i) {
        const float angle = (2.0f * 3.14159265f * static_cast<float>(i)) / static_cast<float>(orbitBulletCount_);
        auto orb = std::make_unique<OrbitBullet>();
        orb->Initialize(player_->GetWorldPosition(), orbitRadius_, angle, orbitAngularSpeed_, orbitBulletScale_, orbitHitInterval_);
        orbitBullets_.push_back(std::move(orb));
    }
}

void PlayerManager::AddDrone() {
    hasDrone_ = true;
    droneLevel_ = 1;
    droneShotCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("drone.lv1.shotCount", 1.0f));
    droneDamageBonus_ = static_cast<int32_t>(GetWeaponUpgradeSetting("drone.lv1.damageBonus", 0.0f));
    dronePierceCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("drone.lv1.pierceCount", 1.0f));
    drone_ = std::make_unique<Drone>();
    drone_->Initialize({ 3.0f, 2.0f, 0.0f });
}

void PlayerManager::UpgradeDrone() {
    if (!hasDrone_) {
        AddDrone();
        return;
    }

    if (droneLevel_ >= kDroneMaxLevel) {
        return;
    }

    ++droneLevel_;
    switch (droneLevel_) {
    case 2:
        droneShotCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("drone.lv2.shotCount", 2.0f));
        break;
    case 3:
        droneInterval_ *= GetWeaponUpgradeSetting("drone.lv3.intervalMultiplier", 0.85f);
        break;
    case 4:
        droneShotCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("drone.lv4.shotCount", 3.0f));
        break;
    case 5:
        droneDamageBonus_ += static_cast<int32_t>(GetWeaponUpgradeSetting("drone.lv5.damageBonusAdd", 1.0f));
        break;
    case 6:
        droneShotCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("drone.lv6.shotCount", 4.0f));
        break;
    case 7:
        dronePierceCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("drone.lv7.pierceCount", 2.0f));
        break;
    case 8:
        droneDamageBonus_ += static_cast<int32_t>(GetWeaponUpgradeSetting("drone.lv8.damageBonusAdd", 1.0f));
        droneInterval_ *= GetWeaponUpgradeSetting("drone.lv8.intervalMultiplier", 0.8f);
        break;
    default:
        break;
    }
}

void PlayerManager::AddLightning() {
    hasLightning_ = true;
    lightningLevel_ = 1;
    lightningStrikeCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("lightning.lv1.strikeCount", 1.0f));
    lightningDamageBonus_ = static_cast<int32_t>(GetWeaponUpgradeSetting("lightning.lv1.damageBonus", 0.0f));
    lightningRadius_ = GetWeaponUpgradeSetting("lightning.lv1.radius", 6.0f);
    lightningInterval_ = GetWeaponUpgradeSetting("lightning.lv1.interval", 2.4f);
    lightningTimer_ = 0.0f;
}

void PlayerManager::UpgradeLightning() {
    if (!hasLightning_) {
        AddLightning();
        return;
    }

    if (lightningLevel_ >= kLightningMaxLevel) {
        return;
    }

    ++lightningLevel_;
    switch (lightningLevel_) {
    case 2:
        lightningDamageBonus_ += static_cast<int32_t>(GetWeaponUpgradeSetting("lightning.lv2.damageBonusAdd", 1.0f));
        break;
    case 3:
        lightningStrikeCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("lightning.lv3.strikeCount", 2.0f));
        break;
    case 4:
        lightningRadius_ += GetWeaponUpgradeSetting("lightning.lv4.radiusAdd", 1.5f);
        break;
    case 5:
        lightningDamageBonus_ += static_cast<int32_t>(GetWeaponUpgradeSetting("lightning.lv5.damageBonusAdd", 1.0f));
        break;
    case 6:
        lightningStrikeCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("lightning.lv6.strikeCount", 3.0f));
        break;
    case 7:
        lightningInterval_ *= GetWeaponUpgradeSetting("lightning.lv7.intervalMultiplier", 0.82f);
        break;
    case 8:
        lightningStrikeCount_ = static_cast<int32_t>(GetWeaponUpgradeSetting("lightning.lv8.strikeCount", 4.0f));
        lightningRadius_ += GetWeaponUpgradeSetting("lightning.lv8.radiusAdd", 1.5f);
        break;
    default:
        break;
    }
}

void PlayerManager::MaxAllWeapons() {
    while (!IsNormalBulletMaxLevel()) {
        UpgradeNormalBullets();
    }

    while (!IsOrbitBulletMaxLevel()) {
        UpgradeOrbitBullets();
    }

    while (!IsDroneMaxLevel()) {
        UpgradeDrone();
    }

    while (!IsLightningMaxLevel()) {
        UpgradeLightning();
    }

    ClearLevelUpRequest();
}

void PlayerManager::UpdateLightning(float deltaTime) {
    if (!hasLightning_ || !enemyManager_) {
        return;
    }

    lightningTimer_ += deltaTime;
    if (lightningTimer_ < lightningInterval_) {
        return;
    }

    lightningTimer_ = 0.0f;
    const std::vector<Vector3> targets = enemyManager_->PickLightningTargets(lightningStrikeCount_);
    const int32_t damage = attackPower_ + lightningDamageBonus_;
    for (const auto& target : targets) {
        auto effect = std::make_unique<LightningStrikeEffect>();
        effect->Initialize(target, lightningRadius_);
        lightningEffects_.push_back(std::move(effect));
        enemyManager_->ApplyLightningDamage(target, lightningRadius_, damage);
    }
}

} // namespace DirectXGame
