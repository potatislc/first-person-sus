#ifndef SCENE_H
#define SCENE_H

namespace Scene {
    class Scene {
    public:
        Scene() = default;

        Scene(const Scene&) = delete;

        Scene& operator=(const Scene&) = delete;

        Scene(Scene&&) = default;

        Scene& operator=(Scene&&) = default;

        virtual ~Scene() = default;

        virtual void onUpdate(float deltaTime) {
        }

        virtual void onRender() {
        }

        virtual void onImGuiRender() {
        }

        // virtual Scene* copy() const = 0;
    };
}

#endif
