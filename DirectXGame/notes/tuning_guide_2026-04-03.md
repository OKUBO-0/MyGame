# 調整ガイド

更新日: 2026-04-03

## 概要

現在、ゲームバランス調整で主に触るCSVは以下の4つ。

- `Resources/data/playerStatus.csv`
- `Resources/data/enemySpawnSettings.csv`
- `Resources/data/levelupWeights.csv`
- `Resources/data/weaponUpgradeSettings.csv`

コードを直接触らなくても、以下の3系統はCSVだけで調整できる。

- 敵の湧き方
- レベルアップ候補の出やすさ
- 武器レベルアップ時の成長量

## 1. playerStatus.csv

用途:

- プレイヤー初期値と基礎成長の上限を調整する。

主なキー:

- `maxLifeStock`
  - 初期最大HP
- `lifeStock`
  - 初期HP
- `attackPower`
  - 初期攻撃力
- `invincibilityDuration`
  - 被弾後の無敵時間
- `normalBulletInterval`
  - 通常弾の初期間隔
- `droneInterval`
  - ドローンの初期間隔
- `maxLifeStockCap`
  - 最大HP強化の上限
- `moveSpeedUpgradeCap`
  - 移動速度強化の上限回数
- `moveSpeedUpgradeStep`
  - 1回あたりの移動速度増分
- `moveSpeedMax`
  - 移動速度の上限

調整の目安:

- 序盤を楽にしたいなら `lifeStock` か `attackPower` を上げる。
- 被弾が重すぎるなら `invincibilityDuration` を少し伸ばす。
- 通常弾が弱すぎるなら `normalBulletInterval` を少し下げる。

## 2. enemySpawnSettings.csv

用途:

- 敵の総数、湧きテンポ、出現位置を調整する。

主なキー:

- `maxActiveEnemies`
  - 同時に存在できる敵の上限
- `spawnUnlockInterval`
  - 新しい敵タイプが解禁される秒数間隔
- `spawnDistance`
  - プレイヤーからどれだけ離れた位置に湧くか
- `respawnDistance`
  - 遠すぎる敵を再配置する判定距離
- `respawnRadius`
  - 再配置時の出現半径
- `minSpawnInterval`
  - 最速時の湧き間隔
- `baseSpawnInterval`
  - 開始時の湧き間隔
- `spawnAcceleration`
  - 時間経過で湧き間隔が詰まる速さ

調整の目安:

- 敵が多すぎて重いなら `maxActiveEnemies` を下げる。
- 中盤以降の圧を上げたいなら `spawnAcceleration` を上げる。
- 序盤をゆるくしたいなら `baseSpawnInterval` を上げる。
- 敵が突然近すぎると感じるなら `spawnDistance` を上げる。

## 3. levelupWeights.csv

用途:

- どのレベルアップ候補が出やすいかを調整する。

基本:

- 数値が高いほど出やすい。
- これは候補抽選の重みであり、確定出現ではない。

主なキー:

- `orbit.unlockWeight`
  - 周囲弾未取得時の出やすさ
- `drone.unlockWeight`
  - ドローン未取得時の出やすさ
- `lightning.unlockWeight`
  - ライトニング未取得時の出やすさ
- `normal.highLevelWeight`
  - 通常弾高レベル時の出やすさ
- `attack.lowWeight`
  - 攻撃力が低いときの出やすさ
- `attack.midWeight`
  - 攻撃力が中盤のときの出やすさ
- `attack.highWeight`
  - 攻撃力が高いときの出やすさ
- `heal.lowHpWeight`
  - HPが少ないときの回復候補の出やすさ
- `heal.midHpWeight`
  - HPが中くらいのときの回復候補の出やすさ
- `heal.highHpWeight`
  - HPが高いときの回復候補の出やすさ

調整の目安:

- ビルド武器を早めに揃えたいなら `*.unlockWeight` を上げる。
- 回復候補が出すぎるなら `heal.*Weight` を下げる。
- 攻撃力候補が終盤でも出続けるなら `attack.highWeight` を下げる。

## 4. weaponUpgradeSettings.csv

用途:

- 各武器の各レベルで何がどれだけ伸びるかを調整する。

命名ルール:

- `weapon.lvX.parameter`

例:

- `normal.lv3.speedMultiplier`
- `orbit.lv4.hitIntervalMultiplier`
- `drone.lv8.intervalMultiplier`
- `lightning.lv6.strikeCount`

### 通常弾

主なキー:

- `normal.lv2.amount`
- `normal.lv3.speedMultiplier`
- `normal.lv3.intervalMultiplier`
- `normal.lv5.damageBonusAdd`
- `normal.lv7.pierceCount`
- `normal.lv8.damageBonusAdd`

調整の目安:

- 前方制圧力を上げたいなら `amount` を増やす。
- 手数を増やしたいなら `intervalMultiplier` を少し下げる。
- 終盤火力を伸ばしたいなら `damageBonusAdd` を上げる。

### 周囲弾

主なキー:

- `orbit.lv2.count`
- `orbit.lv3.radiusAdd`
- `orbit.lv3.angularSpeedAdd`
- `orbit.lv3.scaleAdd`
- `orbit.lv4.hitIntervalMultiplier`
- `orbit.lv8.count`

調整の目安:

- 近距離の安全性を上げたいなら `count` を増やす。
- 見た目の成長感を出したいなら `radiusAdd` と `scaleAdd` を上げる。
- DPSを上げたいなら `hitIntervalMultiplier` をさらに小さくする。

### ドローン

主なキー:

- `drone.lv2.shotCount`
- `drone.lv3.intervalMultiplier`
- `drone.lv5.damageBonusAdd`
- `drone.lv7.pierceCount`
- `drone.lv8.intervalMultiplier`

調整の目安:

- 安定火力を上げたいなら `shotCount` か `damageBonusAdd` を上げる。
- テンポを上げたいなら `intervalMultiplier` を下げる。

### ライトニング

主なキー:

- `lightning.lv1.interval`
- `lightning.lv2.damageBonusAdd`
- `lightning.lv3.strikeCount`
- `lightning.lv4.radiusAdd`
- `lightning.lv7.intervalMultiplier`
- `lightning.lv8.strikeCount`

調整の目安:

- 爽快感を上げたいなら `strikeCount` を増やす。
- 範囲殲滅を強くしたいなら `radiusAdd` を上げる。
- 発動頻度を上げたいなら `interval` または `intervalMultiplier` を下げる。

## 調整の進め方

おすすめの順番:

1. `enemySpawnSettings.csv` で全体難易度を大まかに決める
2. `playerStatus.csv` で序盤の遊びやすさを調整する
3. `weaponUpgradeSettings.csv` で武器ごとの強さを整える
4. `levelupWeights.csv` で候補の出方を整える

## 注意

- 1つの値を大きく動かしすぎると、他のCSV調整意図を壊しやすい。
- `intervalMultiplier` は 1.0 より小さいと高速化、1.0 より大きいと低速化。
- `count` や `amount` を増やしすぎると、見た目だけでなく処理負荷にも影響する。
- CSVが読めない場合でもゲームは既定値で動くようにしてあるが、意図通りに反映されないのでファイル名とキー名は固定で扱う。
