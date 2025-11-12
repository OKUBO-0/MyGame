#pragma once
#include <cstdint>

enum class SCENE {
    Title,
    Game,
    Result,
};

class IScene {
protected:
    static SCENE sceneNo; // 現在のシーン番号

public:
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Finalize() = 0;

    virtual bool IsFinished() const = 0;

    virtual ~IScene();

    SCENE GetSceneNo() const;
    void SetSceneNo(SCENE next) { sceneNo = next; }

    // sceneNoへのアクセス用static関数を追加
    static void SetStaticSceneNo(SCENE next) { sceneNo = next; }
    static SCENE GetStaticSceneNo() { return sceneNo; }
};