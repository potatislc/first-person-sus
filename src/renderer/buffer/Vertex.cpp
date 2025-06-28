#include "Vertex.h"

#include <glad/glad.h>

#include <utility>

#include "../Renderer.h"

Renderer::Buffer::DataBuffer Renderer::Buffer::Vertex::layoutInterleave(
    const Layout& layout, const DataBatch& dataBatch) {
    assert(layout.getAttributes().size() == dataBatch.size());

    DataBuffer combinedBuffer;

    Size minElements{std::numeric_limits<Size>::max()};
    for (Size i = 0; i < dataBatch.size(); i++) {
        const Size attrSize = Shader::dataTypeSize(layout.getAttributes()[i].dataType);
        assert(attrSize > 0);
        assert(dataBatch[i].size() % attrSize == 0); // Are they aligned?
        minElements = std::min(minElements, dataBatch[i].size() / attrSize);
    }

    combinedBuffer.resize(minElements * layout.getStride());

    using DifferenceType = std::_Vector_iterator<std::_Vector_val<std::_Simple_types<unsigned
        char> > >::difference_type;

    for (DifferenceType i = 0; std::cmp_less(i, minElements); i++) {
        DifferenceType offsetInStride = 0;
        for (Size j = 0; j < layout.getAttributes().size(); j++) {
            const auto attributeSize = Shader::dataTypeSize(layout.getAttributes()[j].dataType);
            std::copy_n(dataBatch[j].begin() + i * attributeSize,
                        attributeSize,
                        combinedBuffer.begin() + i * static_cast<DifferenceType>(layout.getStride()) + offsetInStride);
            offsetInStride += attributeSize;
        }
    }

    return combinedBuffer;
}

Renderer::Buffer::Vertex::Vertex(Layout layout, const void* data, const uint32_t size) : m_layout{std::move(layout)} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    bind();
    RENDERER_API_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

Renderer::Buffer::Vertex::Vertex(Layout layout, const DataBatch& dataBatch) : m_layout{std::move(layout)} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    bind();
    const auto batchedDataBuffer = batch(dataBatch);
    RENDERER_API_CALL(
        glBufferData(GL_ARRAY_BUFFER, batchedDataBuffer.size(), batchedDataBuffer.data(), GL_STATIC_DRAW));
}

Renderer::Buffer::Vertex::~Vertex() {
    RENDERER_API_CALL(glDeleteBuffers(1, &m_id));
}

void Renderer::Buffer::Vertex::bind() const {
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
}

void Renderer::Buffer::Vertex::unbind() {
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
