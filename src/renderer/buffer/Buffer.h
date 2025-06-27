#pragma once
#include <vector>
#include "../../core/Typedef.h"

namespace Renderer::Buffer {
    using Count = uint32_t;
    using Size = size_t;

    struct DataConfig {
        const void* data{};
        Size size{};
    };

    using DataBatch = std::vector<DataConfig>;

    using DataBuffer = std::vector<uint8_t>;

    inline DataBuffer batch(const DataBatch& dataBatch) {
        Size totalSize{};
        for (const auto [_, size]: dataBatch) {
            totalSize += size;
        }

        DataBuffer batchedDataBuffer;
        batchedDataBuffer.reserve(totalSize);
        Size offset{};
        for (const auto& [data, size]: dataBatch) {
            batchedDataBuffer.resize(offset + size);
            std::memcpy(batchedDataBuffer.data() + offset, data, size);
            offset += size;
        }

        return batchedDataBuffer;
    }
}
