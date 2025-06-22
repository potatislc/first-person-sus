#include "Uniform.h"

#include <glad/glad.h>
#include "../Renderer.h"

bool Renderer::Shader::Uniform::locate(const uint32_t programId) {
    m_Location = RENDERER_API_CALL_RETURN(glGetUniformLocation(programId, m_Name.c_str()));

    if (m_Location == noLocation) {
        std::cerr << "Could not locate uniform of name: " << m_Name << ". With program id: " << programId << ".\n";
        ASSERT(false);
    }

    return m_Location != noLocation;
}
