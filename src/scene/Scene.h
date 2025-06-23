#ifndef SCENE_H
#define SCENE_H

namespace Scene {
    class Scene {
    public:
        Scene() = default;

        Scene(const Scene&) = delete;

        Scene& operator =(const Scene&) = delete;

        Scene(Scene&&) = default;

        Scene& operator=(Scene&&) = default;

        virtual ~Scene() = default;

        virtual void onUpdate() = 0;

        virtual void onRender() = 0;

        virtual void onImGuiRender() {
        }
    };
}

#endif
