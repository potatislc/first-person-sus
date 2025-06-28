#pragma once
#include <vector>
#include "../../core/Typedef.h"

namespace Renderer::Buffer {
    using Count = uint32_t;
    using Size = size_t;

    using DataBuffer = std::vector<uint8_t>;

    using DataBatch = std::vector<DataBuffer>;

    inline DataBuffer copyDataBuffer(const void* data, const Size size) {
        DataBuffer dataBuffer(size);
        std::memcpy(dataBuffer.data(), data, dataBuffer.size());
        return dataBuffer;
    }

    inline DataBuffer copyDataBuffer(const DataBuffer& other) {
        return copyDataBuffer(other.data(), other.size());
    }

    inline DataBuffer batch(const DataBatch& dataBatch) {
        Size totalSize{};
        for (const auto& buffer: dataBatch) {
            totalSize += buffer.size();
        }

        DataBuffer batchedDataBuffer;
        batchedDataBuffer.reserve(totalSize);
        ptrdiff_t offset{};
        for (const auto& buffer: dataBatch) {
            batchedDataBuffer.resize(offset + buffer.size());
            std::copy_n(buffer.begin(), buffer.size(), batchedDataBuffer.begin() + offset);
            offset += static_cast<ptrdiff_t>(buffer.size());
        }

        return batchedDataBuffer;
    }
}
