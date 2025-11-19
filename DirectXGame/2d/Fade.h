#pragma once

#include <KamataEngine.h>

/// <summary>
/// 画面のフェードイン／フェードアウトを制御するクラス。
/// スプライトのアルファ値を操作して、シーン遷移や演出に利用する。
/// </summary>
class Fade {
public:
    /// <summary>
    /// フェードの状態を表す列挙型
    /// None: フェードなし
    /// FadeIn: フェードイン中
    /// Stay: 完全表示状態で待機
    /// FadeOut: フェードアウト中
    /// </summary>
    enum class State {
        None,
        FadeIn,
        Stay,
        FadeOut,
    };

    /// <summary>
    /// コンストラクタ
    /// フェード用スプライトの初期値を設定する
    /// </summary>
    Fade();

    /// <summary>
    /// デストラクタ
    /// リソースの解放を行う
    /// </summary>
    ~Fade();

    /// <summary>
    /// フェード処理の初期化を行う
    /// スプライト生成や初期アルファ値の設定を行う
    /// </summary>
    void Initialize();

    /// <summary>
    /// フェード状態の更新処理
    /// 状態に応じてアルファ値を変化させる
    /// </summary>
    void Update();

    /// <summary>
    /// フェードスプライトの描画処理
    /// 現在のアルファ値に基づいて画面に表示する
    /// </summary>
    void Draw();

    /// <summary>
    /// フェードインを開始する
    /// </summary>
    /// <param name="speed">アルファ値の変化速度（デフォルト0.02f）</param>
    void StartFadeIn(float speed = 0.02f);

    /// <summary>
    /// フェードアウトを開始する
    /// </summary>
    /// <param name="speed">アルファ値の変化速度（デフォルト0.02f）</param>
    void StartFadeOut(float speed = 0.02f);

    /// <summary>
    /// 現在のフェード状態を取得する
    /// </summary>
    /// <returns>フェードの状態（State列挙型）</returns>
    State GetState() const { return state_; }

    /// <summary>
    /// フェード処理が完了しているかを判定する
    /// </summary>
    /// <returns>true: 完了 / false: 未完了</returns>
    bool IsFinished() const { return finished_; }

private:
    KamataEngine::Sprite* fadeSprite_ = nullptr; ///< フェード用スプライト
    float alpha_ = 1.0f;                         ///< 現在のアルファ値
    float speed_ = 0.02f;                        ///< アルファ値の変化速度
    State state_ = State::None;                  ///< 現在のフェード状態
    bool finished_ = false;                      ///< フェード完了フラグ
};