#include "EnemyManager.h"
#include "../player/PlayerManager.h"
#include <fstream>
#include <sstream>
#include <cmath>

using namespace KamataEngine;

void EnemyManager::Initialize(const std::string& csvPath, Player* player, PlayerManager* playerManager) {
    player_ = player;
    playerManager_ = playerManager;
    SpawnEnemiesFromCSV(csvPath);

    audio_ = Audio::GetInstance();
    hitSEHandle_ = Audio::GetInstance()->LoadWave("Sounds/se_hit.wav");

    // 追加: PlayerManager 側に this を登録して相互参照を完成させる
    if (playerManager_) {
        playerManager_->SetEnemyManager(this);
    }
}

void EnemyManager::SpawnEnemiesFromCSV(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        OutputDebugStringA(("CSV読み込み失敗: " + filePath + "\n").c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        int32_t type = 0;
        int32_t count = 0;
        int32_t hp = 3;
        int32_t exp = 0;
        float distance = 0.0f;

        std::getline(ss, value, ','); type = std::stoi(value);
        std::getline(ss, value, ','); distance = std::stof(value);
        std::getline(ss, value, ','); count = std::stoi(value);
        std::getline(ss, value, ','); hp = std::stoi(value);
        std::getline(ss, value, ','); exp = std::stoi(value);

        for (int32_t i = 0; i < count; ++i) {
            float angle = (float(rand()) / RAND_MAX) * 2.0f * 3.14159265f;
            Vector3 pos = {
                player_->GetWorldPosition().x + std::cos(angle) * distance,
                0.0f,
                player_->GetWorldPosition().z + std::sin(angle) * distance
            };

            auto enemy = std::make_unique<Enemy>();
            enemy->SetHP(hp);
            enemy->SetEXP(exp);
            enemy->Initialize();
            enemy->SetPlayer(player_);
            enemy->SetPosition(pos);
            enemy->SetModelByType(type);
            enemies_.push_back(std::move(enemy));
        }
    }

    file.close();
}

void EnemyManager::Update() {

    for (auto& enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Update();
        }
        else if (enemy->GetHP() <= 0 && enemy->JustDied()) {

            GameData::totalKillCount++;

            auto orb = std::make_unique<ExpOrb>();
            orb->Initialize(enemy->GetPosition(), enemy->GetEXP());
            expOrbs_.push_back(std::move(orb));

            const int particleCount = 5;
            for (int i = 0; i < particleCount; ++i) {
                auto p = std::make_unique<DeathParticle>();
                p->Initialize(enemy->GetPosition());
                deathParticles_.push_back(std::move(p));
            }

            enemy->ResetJustDied();
        }
    }

    {
        Vector3 pPos = player_->GetWorldPosition();

        float despawnDist = 75.0f;
        float respawnRadius = 60.0f;

        for (auto& enemy : enemies_) {
            if (!enemy->IsActive()) continue;

            Vector3 ePos = enemy->GetPosition();
            float dx = ePos.x - pPos.x;
            float dz = ePos.z - pPos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq > despawnDist * despawnDist) {

                float angle = (float(rand()) / RAND_MAX) * 2.0f * 3.14159265f;

                Vector3 newPos = {
                    pPos.x + std::cos(angle) * respawnRadius,
                    0.0f,
                    pPos.z + std::sin(angle) * respawnRadius
                };

                enemy->SetPosition(newPos);
            }
        }
    }


    for (auto it = deathParticles_.begin(); it != deathParticles_.end();) {
        (*it)->Update();
        if (!(*it)->IsActive()) {
            it = deathParticles_.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto it = expOrbs_.begin(); it != expOrbs_.end();) {
        (*it)->Update(player_->GetWorldPosition());
        if (!(*it)->IsActive()) {
            playerManager_->AddEXP((*it)->GetEXP());
            it = expOrbs_.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto it = hitParticles_.begin(); it != hitParticles_.end();) {
        (*it)->Update();
        if (!(*it)->IsActive()) {
            it = hitParticles_.erase(it);
        }
        else {
            ++it;
        }
    }

    const float kMinDist = 3.0f;
    const float kPushStrength = 1.0f;

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

            if (distSq < kMinDist * kMinDist && distSq > 0.0001f) {
                float dist = std::sqrt(distSq);
                float overlap = kMinDist - dist;

                float nx = dx / dist;
                float nz = dz / dist;

                posA.x -= nx * overlap * kPushStrength;
                posA.z -= nz * overlap * kPushStrength;
                posB.x += nx * overlap * kPushStrength;
                posB.z += nz * overlap * kPushStrength;

                a->SetPosition(posA);
                b->SetPosition(posB);
            }
        }
    }
}

void EnemyManager::CheckCollisions(Player* player, PlayerManager* playerManager)
{
    int damage = playerManager->GetAttackPower();

    // ============================
    // NormalBullet と敵の当たり判定
    // ============================
    for (auto& bullet : playerManager->GetNormalBullets()) {
        if (!bullet->IsActive()) continue;

        for (auto& enemy : enemies_) {
            if (!enemy->IsActive()) continue;

            Vector3 bPos = bullet->GetPosition();
            Vector3 ePos = enemy->GetPosition();

            float dx = bPos.x - ePos.x;
            float dz = bPos.z - ePos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq < 4.0f) {

                if (!bullet->CanHitEnemy(enemy.get())) continue;

                Audio::GetInstance()->PlayWave(hitSEHandle_, false, 0.5f);
                bullet->RegisterHit(enemy.get());

                Vector3 knockDir = { ePos.x - bPos.x, 0, ePos.z - bPos.z };
                float len = std::sqrt(knockDir.x * knockDir.x + knockDir.z * knockDir.z);
                if (len > 0.0f) {
                    knockDir.x /= len;
                    knockDir.z /= len;
                }

                float knockStrength = 0.6f + damage * 0.15f;

                enemy->TakeDamage(damage, knockDir, knockStrength);

                for (int i = 0; i < 4; ++i) {
                    auto spark = std::make_unique<HitParticle>();
                    spark->Initialize(bPos);
                    hitParticles_.push_back(std::move(spark));
                }

                bullet->Deactivate();
            }
        }
    }

    // ============================
    // OrbitBullet と敵の当たり判定
    // ============================
    for (auto& orb : playerManager->GetOrbitBullets()) {
        if (!orb->IsActive()) continue;

        for (auto& enemy : enemies_) {
            if (!enemy->IsActive()) continue;

            Vector3 oPos = orb->GetPosition();
            Vector3 ePos = enemy->GetPosition();

            float dx = oPos.x - ePos.x;
            float dz = oPos.z - ePos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq < 25.0f) {

                if (!orb->CanHitEnemy(enemy.get())) continue;

                Audio::GetInstance()->PlayWave(hitSEHandle_, false, 0.5f);
                orb->RegisterHit(enemy.get());

                Vector3 knockDir = { ePos.x - oPos.x, 0, ePos.z - oPos.z };
                float len = std::sqrt(knockDir.x * knockDir.x + knockDir.z * knockDir.z);
                if (len > 0.0f) {
                    knockDir.x /= len;
                    knockDir.z /= len;
                }

                float knockStrength = 0.5f + damage * 0.1f;

                enemy->TakeDamage(damage, knockDir, knockStrength);

                for (int i = 0; i < 4; ++i) {
                    auto spark = std::make_unique<HitParticle>();
                    spark->Initialize(oPos);
                    hitParticles_.push_back(std::move(spark));
                }
            }
        }
    }

    // ===========================
	// ドローンの弾と敵の当たり判定
    // ============================
    if (playerManager->HasDrone()) {
        auto& drone = playerManager->GetDrone();

        for (auto& bullet : drone->GetBullets()) {
            if (!bullet->IsActive()) continue;

            for (auto& enemy : enemies_) {
                if (!enemy->IsActive()) continue;

                Vector3 bPos = bullet->GetPosition();
                Vector3 ePos = enemy->GetPosition();

                float dx = bPos.x - ePos.x;
                float dz = bPos.z - ePos.z;
                float distSq = dx * dx + dz * dz;

                if (distSq < 4.0f) {

                    Vector3 knockDir = { ePos.x - bPos.x, 0, ePos.z - bPos.z };
                    float len = std::sqrt(knockDir.x * knockDir.x + knockDir.z * knockDir.z);
                    if (len > 0.0f) {
                        knockDir.x /= len;
                        knockDir.z /= len;
                    }

                    int droneDamage = playerManager->GetAttackPower() / 2;

                    enemy->TakeDamage(droneDamage, knockDir, 0.5f);

                    bullet->Deactivate();
                }
            }
        }
    }

    // ============================
    // プレイヤーと敵の接触判定
    // ============================
    Vector3 pPos = player->GetWorldPosition();

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive()) continue;

        Vector3 ePos = enemy->GetPosition();
        float dx = ePos.x - pPos.x;
        float dz = ePos.z - pPos.z;
        float distSq = dx * dx + dz * dz;

        const float minDist = 3.0f;

        if (distSq < minDist * minDist && distSq > 0.0001f) {

            float dist = std::sqrt(distSq);
            float overlap = minDist - dist;

            float nx = dx / dist;
            float nz = dz / dist;

            ePos.x += nx * overlap;
            ePos.z += nz * overlap;
            enemy->SetPosition(ePos);

            if (!playerManager->IsInvincible()) {
                playerManager->TakeDamage();
                Audio::GetInstance()->PlayWave(hitSEHandle_, false, 0.5f);
            }
        }
    }
}

void EnemyManager::Draw(Camera* camera) {
    for (auto& enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Draw(camera);
        }
    }

    for (auto& orb : expOrbs_) {
        orb->Draw(camera);
    }

    for (auto& p : deathParticles_) {
        p->Draw(camera);
    }
}

void EnemyManager::DrawHitParticles(Camera* camera) {
    for (auto& p : hitParticles_) {
        p->Draw(camera);
    }
}