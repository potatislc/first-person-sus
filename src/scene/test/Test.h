
#ifndef TESTSCENE_H
#define TESTSCENE_H

#include "../Scene.h"

namespace Scene {
    class Test final : Scene {
    public:
        void onUpdate() override;

        void onRender() override;

        void onImGuiRender() override;
    };
}


#endif
