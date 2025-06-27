#pragma once
#include <vector>

namespace Renderer::Buffer {
    struct DataConfig {
        const void* data{};
        uint32_t size{};
    };

    using DataBatch = std::vector<DataConfig>;

    using DataBuffer = std::vector<uint8_t>;

    inline DataBuffer batch(const DataBatch& dataBatch) {
        size_t totalSize{};
        for (const auto [_, size]: dataBatch) {
            totalSize += size;
        }

        DataBuffer batchedDataBuffer;
        batchedDataBuffer.reserve(totalSize);
        size_t offset{};
        for (const auto& [data, size]: dataBatch) {
            batchedDataBuffer.resize(offset + size);
            std::memcpy(batchedDataBuffer.data() + offset, data, size);
            offset += size;
        }

        return batchedDataBuffer;
    }
}
