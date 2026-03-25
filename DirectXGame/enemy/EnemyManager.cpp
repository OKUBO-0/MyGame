#include "EnemyManager.h"
#include "../player/core/PlayerManager.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace KamataEngine;

namespace DirectXGame {

size_t EnemyManager::GetActiveEnemyCount() const {
    return static_cast<size_t>(std::count_if(enemies_.begin(), enemies_.end(),
        [](const std::unique_ptr<Enemy>& enemy) {
            return enemy && enemy->IsActive();
        }));
}

void EnemyManager::Initialize(const std::string& csvPath, Player* player, PlayerManager* playerManager) {
    player_ = player;
    playerManager_ = playerManager;

    // 敵タイプ定義を読み込む
    LoadEnemyTypes(csvPath);

    audio_ = Audio::GetInstance();
    hitSEHandle_ = audio_->LoadWave("audio/se/se_hit.wav");
    playerDamageSEHandle_ = audio_->LoadWave("audio/se/se_hit.wav");

    if (playerManager_) {
        playerManager_->SetEnemyManager(this);
    }
}

void EnemyManager::LoadEnemyTypes(const std::string& filePath) {
    enemyTypes_.clear();

    std::ifstream file(filePath);
    if (!file.is_open()) {
        OutputDebugStringA(("CSV読み込み失敗: " + filePath + "\n").c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string value;

        EnemyTypeData data{};

        // CSV: type, baseHP, baseSpeed, baseEXP, spawnCount
        std::getline(ss, value, ','); data.type = std::stoi(value);
        std::getline(ss, value, ','); data.baseHP = std::stoi(value);
        std::getline(ss, value, ','); data.baseSpeed = std::stof(value);
        std::getline(ss, value, ','); data.baseEXP = std::stoi(value);
        std::getline(ss, value, ','); data.spawnCount = std::stoi(value);

        enemyTypes_.push_back(data);
    }

    file.close();
}

void EnemyManager::SpawnEnemies() {
    if (enemyTypes_.empty() || !player_) return;
    if (GetActiveEnemyCount() >= kMaxActiveEnemies) return;

    // 経過時間で解禁される敵タイプを増やす（20秒ごとに1タイプ解禁）
    int maxIndex = static_cast<int>(elapsedTime_ / kSpawnUnlockInterval);
    maxIndex = std::clamp(maxIndex, 0, (int)enemyTypes_.size() - 1);

    const EnemyTypeData& data = enemyTypes_[rand() % (maxIndex + 1)];

    for (int i = 0; i < data.spawnCount; i++) {
        if (GetActiveEnemyCount() >= kMaxActiveEnemies) {
            break;
        }
        SpawnOneEnemy(data);
    }
}

void EnemyManager::SpawnOneEnemy(const EnemyTypeData& data) {
    Vector3 pPos = player_->GetWorldPosition();

    float angle = (float(rand()) / RAND_MAX) * 2.0f * 3.14159265f;

    Vector3 pos = {
        pPos.x + std::cos(angle) * kSpawnDistance,
        0.0f,
        pPos.z + std::sin(angle) * kSpawnDistance
    };

    auto enemy = std::make_unique<Enemy>();
    enemy->Initialize();
    enemy->SetPlayer(player_);
    enemy->SetPosition(pos);
    enemy->SetModelByType(data.type);
    enemy->SetBehaviorByType(data.type);

    // 時間経過で強化
    int hp = data.baseHP + (int)(elapsedTime_ / 30.0f);
    int exp = data.baseEXP + (int)(elapsedTime_ / 40.0f);
    float speed = data.baseSpeed + elapsedTime_ * 0.002f;

    enemy->SetHP(hp);
    enemy->SetEXP(exp);

    // Enemy に速度設定メソッドがある前提
    enemy->SetSpeed(speed);

    enemies_.push_back(std::move(enemy));
}

void EnemyManager::UpdateSpawnState() {
    elapsedTime_ += kDeltaTime;
    spawnTimer_ += kDeltaTime;
    spawnInterval_ = std::max<float>(kMinSpawnInterval, kBaseSpawnInterval - elapsedTime_ * kSpawnAcceleration);

    if (spawnTimer_ >= spawnInterval_) {
        SpawnEnemies();
        spawnTimer_ = 0.0f;
    }
}

void EnemyManager::SpawnDeathEffects(const Enemy& enemy) {
    ++totalKillCount_;

    auto orb = std::make_unique<ExpOrb>();
    orb->Initialize(enemy.GetPosition(), enemy.GetEXP());
    expOrbs_.push_back(std::move(orb));

    for (int32_t i = 0; i < kDeathParticleSpawnCount; ++i) {
        if (deathParticles_.size() >= kMaxDeathParticles) {
            deathParticles_.pop_front();
        }
        auto particle = std::make_unique<DeathParticle>();
        particle->Initialize(enemy.GetPosition());
        deathParticles_.push_back(std::move(particle));
    }
}

void EnemyManager::UpdateEnemies() {
    for (auto& enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Update();
        }
        else if (enemy->GetHP() <= 0 && enemy->JustDied()) {
            SpawnDeathEffects(*enemy);
            enemy->ResetJustDied();
        }
    }
}

void EnemyManager::RemoveInactiveEnemies() {
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& enemy) {
                return enemy && !enemy->IsActive() && !enemy->JustDied();
            }),
        enemies_.end());
}

void EnemyManager::RelocateFarEnemies() {
    Vector3 playerPosition = player_->GetWorldPosition();

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive()) {
            continue;
        }

        Vector3 enemyPosition = enemy->GetPosition();
        float dx = enemyPosition.x - playerPosition.x;
        float dz = enemyPosition.z - playerPosition.z;
        float distSq = dx * dx + dz * dz;

        if (distSq <= kRespawnDistance * kRespawnDistance) {
            continue;
        }

        float angle = (float(rand()) / RAND_MAX) * 2.0f * 3.14159265f;
        Vector3 newPos = {
            playerPosition.x + std::cos(angle) * kRespawnRadius,
            0.0f,
            playerPosition.z + std::sin(angle) * kRespawnRadius
        };
        enemy->SetPosition(newPos);
    }
}

void EnemyManager::UpdateEffects() {
    for (auto it = deathParticles_.begin(); it != deathParticles_.end();) {
        (*it)->Update();
        if (!(*it)->IsActive()) it = deathParticles_.erase(it);
        else ++it;
    }

    for (auto it = expOrbs_.begin(); it != expOrbs_.end();) {
        (*it)->Update(player_->GetWorldPosition());
        if (!(*it)->IsActive()) {
            playerManager_->AddEXP((*it)->GetEXP());
            it = expOrbs_.erase(it);
        }
        else ++it;
    }

    for (auto it = hitParticles_.begin(); it != hitParticles_.end();) {
        (*it)->Update();
        if (!(*it)->IsActive()) it = hitParticles_.erase(it);
        else ++it;
    }
}

void EnemyManager::ResolveEnemySeparation() {
    for (size_t i = 0; i < enemies_.size(); ++i) {
        Enemy* a = enemies_[i].get();
        if (!a->IsActive()) continue;

        for (size_t j = i + 1; j < enemies_.size(); ++j) {
            Enemy* b = enemies_[j].get();
            if (!b->IsActive()) continue;

            Vector3 posA = a->GetPosition();
            Vector3 posB = b->GetPosition();

            float dx = posB.x - posA.x;
            float dz = posB.z - posA.z;
            float distSq = dx * dx + dz * dz;

            if (distSq < kEnemySeparationDistance * kEnemySeparationDistance && distSq > 0.0001f) {
                float dist = std::sqrt(distSq);
                float overlap = kEnemySeparationDistance - dist;

                float nx = dx / dist;
                float nz = dz / dist;

                posA.x -= nx * overlap * kEnemySeparationStrength;
                posA.z -= nz * overlap * kEnemySeparationStrength;
                posB.x += nx * overlap * kEnemySeparationStrength;
                posB.z += nz * overlap * kEnemySeparationStrength;

                a->SetPosition(posA);
                b->SetPosition(posB);
            }
        }
    }
}

void EnemyManager::Update() {
    UpdateSpawnState();
    UpdateEnemies();
    RemoveInactiveEnemies();
    RelocateFarEnemies();
    UpdateEffects();
    ResolveEnemySeparation();
}

void EnemyManager::SpawnHitParticles(const Vector3& position) {
    for (int32_t i = 0; i < kHitParticleSpawnCount; ++i) {
        if (hitParticles_.size() >= kMaxHitParticles) {
            hitParticles_.pop_front();
        }
        auto spark = std::make_unique<HitParticle>();
        spark->Initialize(position);
        hitParticles_.push_back(std::move(spark));
    }
}

bool EnemyManager::TryHandleBulletHit(Enemy& enemy, const Vector3& impactPosition, int32_t damage, float knockStrength) {
    if (audio_) {
        audio_->PlayWave(hitSEHandle_, false, 0.5f);
    }

    Vector3 enemyPosition = enemy.GetPosition();
    Vector3 knockDir = { enemyPosition.x - impactPosition.x, 0, enemyPosition.z - impactPosition.z };
    float len = std::sqrt(knockDir.x * knockDir.x + knockDir.z * knockDir.z);
    if (len > 0.0f) {
        knockDir.x /= len;
        knockDir.z /= len;
    }

    enemy.TakeDamage(damage, knockDir, knockStrength);
    SpawnHitParticles(impactPosition);
    return true;
}

void EnemyManager::CheckNormalBulletCollisions(PlayerManager& playerManager) {
    int32_t damage = playerManager.GetAttackPower();

    for (auto& bullet : playerManager.GetNormalBullets()) {
        if (!bullet->IsActive()) continue;

        for (auto& enemy : enemies_) {
            if (!enemy->IsActive()) continue;

            Vector3 bPos = bullet->GetPosition();
            Vector3 ePos = enemy->GetPosition();

            float dx = bPos.x - ePos.x;
            float dz = bPos.z - ePos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq < kNormalBulletHitDistanceSq) {
                if (!bullet->CanHitEnemy(enemy.get())) continue;

                bullet->RegisterHit(enemy.get());
                TryHandleBulletHit(*enemy, bPos, damage, 0.6f + damage * 0.15f);
                bullet->Deactivate();
                break;
            }
        }
    }
}

void EnemyManager::CheckOrbitBulletCollisions(PlayerManager& playerManager) {
    int32_t damage = playerManager.GetAttackPower();

    for (auto& orb : playerManager.GetOrbitBullets()) {
        if (!orb->IsActive()) continue;

        for (auto& enemy : enemies_) {
            if (!enemy->IsActive()) continue;

            Vector3 oPos = orb->GetPosition();
            Vector3 ePos = enemy->GetPosition();

            float dx = oPos.x - ePos.x;
            float dz = oPos.z - ePos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq < kOrbitBulletHitDistanceSq) {
                if (!orb->CanHitEnemy(enemy.get())) continue;

                orb->RegisterHit(enemy.get());
                TryHandleBulletHit(*enemy, oPos, damage, 0.5f + damage * 0.1f);
            }
        }
    }
}

void EnemyManager::CheckDroneBulletCollisions(PlayerManager& playerManager) {
    if (!playerManager.HasDrone()) {
        return;
    }

    auto& drone = playerManager.GetDrone();

    for (auto& bullet : drone->GetBullets()) {
        if (!bullet->IsActive()) continue;

        for (auto& enemy : enemies_) {
            if (!enemy->IsActive()) continue;

            Vector3 bPos = bullet->GetPosition();
            Vector3 ePos = enemy->GetPosition();

            float dx = bPos.x - ePos.x;
            float dz = bPos.z - ePos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq < kNormalBulletHitDistanceSq) {
                int32_t droneDamage = playerManager.GetAttackPower() / 2;
                TryHandleBulletHit(*enemy, bPos, droneDamage, 0.5f);
                bullet->Deactivate();
                break;
            }
        }
    }
}

void EnemyManager::CheckPlayerCollisions(Player& player, PlayerManager& playerManager) {
    Vector3 playerPosition = player.GetWorldPosition();

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive()) continue;

        Vector3 ePos = enemy->GetPosition();
        float dx = ePos.x - playerPosition.x;
        float dz = ePos.z - playerPosition.z;
        float distSq = dx * dx + dz * dz;
        if (distSq < kPlayerContactDistanceSq && distSq > 0.0001f) {

            float dist = std::sqrt(distSq);
            float overlap = kPlayerContactDistance - dist;

            float nx = dx / dist;
            float nz = dz / dist;

            ePos.x += nx * overlap;
            ePos.z += nz * overlap;
            enemy->SetPosition(ePos);

            if (!playerManager.IsInvincible()) {
                playerManager.TakeDamage();
                if (audio_) {
                    audio_->PlayWave(playerDamageSEHandle_, false, 0.8f);
                }
            }
        }
    }
}

void EnemyManager::CheckCollisions(Player* player, PlayerManager* playerManager) {
    if (!player || !playerManager) {
        return;
    }

    CheckNormalBulletCollisions(*playerManager);
    CheckOrbitBulletCollisions(*playerManager);
    CheckDroneBulletCollisions(*playerManager);
    CheckPlayerCollisions(*player, *playerManager);
}

void EnemyManager::Draw(Camera* camera) {
    for (auto& enemy : enemies_) {
        if (enemy->IsActive()) enemy->Draw(camera);
    }
    for (auto& orb : expOrbs_) orb->Draw(camera);
    for (auto& p : deathParticles_) p->Draw(camera);
}

void EnemyManager::DrawHitParticles(Camera* camera) {
    for (auto& p : hitParticles_) p->Draw(camera);
}

} // namespace DirectXGame
