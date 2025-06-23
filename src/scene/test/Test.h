
#ifndef TESTSCENE_H
#define TESTSCENE_H

#include "../Scene.h"

namespace Scene {
    class Test final : public Scene {
    public:
        Test();

        void onUpdate(float deltaTime) override;

        void onRender() override;

        void onImGuiRender() override;
    };
}


#endif
