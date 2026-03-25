#include "EnemyManager.h"
#include "../player/PlayerManager.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace KamataEngine;

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
    hitSEHandle_ = audio_->LoadWave("Sounds/se_hit.wav");
    playerDamageSEHandle_ = audio_->LoadWave("Sounds/se_hit.wav");

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
    int maxIndex = static_cast<int>(elapsedTime_ / 20.0f);
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
    float distance = 50.0f;

    Vector3 pos = {
        pPos.x + std::cos(angle) * distance,
        0.0f,
        pPos.z + std::sin(angle) * distance
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

void EnemyManager::Update() {
    const float dt = 1.0f / 60.0f;

    elapsedTime_ += dt;
    spawnTimer_ += dt;

    // スポーン間隔を短くする（下限 0.5秒）
    spawnInterval_ = std::max<float>(0.5f, 2.0f - elapsedTime_ * 0.01f);

    // 一定間隔で敵を湧かせる
    if (spawnTimer_ >= spawnInterval_) {
        SpawnEnemies();
        spawnTimer_ = 0.0f;
    }

    // 敵の更新・死亡処理
    for (auto& enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Update();
        }
        else if (enemy->GetHP() <= 0 && enemy->JustDied()) {

            GameData::totalKillCount++;

            auto orb = std::make_unique<ExpOrb>();
            orb->Initialize(enemy->GetPosition(), enemy->GetEXP());
            expOrbs_.push_back(std::move(orb));

            for (int i = 0; i < 5; i++) {
                if (deathParticles_.size() >= kMaxDeathParticles) {
                    deathParticles_.pop_front();
                }
                auto p = std::make_unique<DeathParticle>();
                p->Initialize(enemy->GetPosition());
                deathParticles_.push_back(std::move(p));
            }

            enemy->ResetJustDied();
        }
    }

    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& enemy) {
                return enemy && !enemy->IsActive() && !enemy->JustDied();
            }),
        enemies_.end());

    // プレイヤーから離れすぎた敵を再配置
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

    // パーティクル・オーブ更新（既存処理）
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

    // 敵同士の押し戻し（既存処理）
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
                    if (hitParticles_.size() >= kMaxHitParticles) {
                        hitParticles_.pop_front();
                    }
                    auto spark = std::make_unique<HitParticle>();
                    spark->Initialize(bPos);
                    hitParticles_.push_back(std::move(spark));
                }

                bullet->Deactivate();
                break;
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
                    if (hitParticles_.size() >= kMaxHitParticles) {
                        hitParticles_.pop_front();
                    }
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
                    break;
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
                Audio::GetInstance()->PlayWave(playerDamageSEHandle_, false, 0.8f);
            }
        }
    }
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
