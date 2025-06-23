
#ifndef TESTSCENE_H
#define TESTSCENE_H

#include <array>
#include <glm/vec2.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../Scene.h"
#include "../../renderer/VertexArray.h"
#include "../../renderer/buffer/Index.h"
#include "../../renderer/shader/Program.h"
#include "../../renderer/texture/Texture.h"

class Application;

namespace Scene {
    class Test final : public Scene {
    public:
        explicit Test(const Application& application);

        void update(float deltaTime) override;

        void render(const Renderer::Renderer& renderer) override;

        void renderImGui() override;

    private:
        static constexpr std::array<glm::vec2, 8> m_Square{
            {
                {-50.f, -50.f}, {0.f, 0.f}, // Top-left
                {50.f, -50.f}, {1.f, 0.f}, // Top-right
                {50.f, 50.f}, {1.f, 1.f}, // Bottom-right
                {-50.f, 50.f}, {0.f, 1.f} // Bottom-left
            }
        };

        static constexpr std::array<uint32_t, 6> m_Indices = {
            0, 1, 2,
            2, 3, 0
        };

        const Application* m_Application{};
        const Renderer::Buffer::Vertex m_VertexBuffer{m_Square.data(), sizeof(m_Square)};
        Renderer::Buffer::Index m_IndexBuffer{m_Indices.data(), m_Indices.size()};
        Renderer::VertexArray m_VertexArray{};
        Renderer::Buffer::Vertex::Layout m_VertexLayout{};
        Renderer::Shader::Program shaderProgram{};
        Renderer::Texture texture{};
        glm::mat4 m_Projection = glm::ortho(0.f, 960.f, 0.f, 540.f, -1.f, 1.f);
        glm::mat4 m_View = glm::translate(glm::mat4{1.f}, glm::vec3{0, 0, 0});
        glm::vec3 translation{200, 200, 0};
    };
}


#endif
