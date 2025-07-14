#include "Uniform.h"

#include <glad/glad.h>
#include "../Renderer.h"

bool Engine::Renderer::Shader::Uniform::locate(const uint32_t programId) {
    m_location = RENDERER_API_CALL_RETURN(glGetUniformLocation(programId, m_name.c_str()));

    if (m_location == noLocation) {
        LOG_ERR(
            "Could not locate uniform of name: " << m_name << ". With program id: " << programId <<
            ". Uniform may not be used in the shader body.\n");
        ASSERT(false);
    }

    return m_location != noLocation;
}
