#pragma once
#include <algorithm>
#include <cassert>
#include <vector>
#include "../../core/Typedef.h"

namespace Engine::Renderer::Buffer {
    using BufferData = std::vector<uint8_t>;
    using IndexData = std::vector<uint32_t>;

    inline BufferData copyBufferData(const void* data, const size_t size) {
        BufferData dataBuffer(size);
        std::memcpy(dataBuffer.data(), data, dataBuffer.size());
        return dataBuffer;
    }

    inline BufferData copyBufferData(const BufferData& bufferData) {
        return BufferData{bufferData};
    }


    inline BufferData batchBufferData(const std::vector<BufferData>& bufferDatas) {
        size_t totalSize{};
        for (const auto& buffer: bufferDatas) {
            totalSize += buffer.size();
        }

        BufferData batchedDataBuffer;
        batchedDataBuffer.reserve(totalSize);
        size_t offset{};
        for (const auto& buffer: bufferDatas) {
            batchedDataBuffer.resize(offset + buffer.size()); // Why?
            std::copy_n(buffer.begin(), buffer.size(), batchedDataBuffer.begin() + static_cast<ptrdiff_t>(offset));
            offset += buffer.size();
        }

        return batchedDataBuffer;
    }

    inline IndexData copyIndexData(const void* data, const size_t count, const size_t instances = 1) {
        assert(instances > 0);
        IndexData indexData(count * instances);

        for (size_t i = 0; i < instances; i++) {
            std::memcpy(indexData.data() + count * i, data, count * sizeof(IndexData::value_type));
        }

        if (instances == 1) {
            return indexData;
        }

        const uint32_t vertexCount{*std::ranges::max_element(indexData) + 1};

        for (size_t i{1}; i < instances; i++) {
            auto begin = indexData.begin() + static_cast<ptrdiff_t>(count * i);
            const auto end = begin + static_cast<ptrdiff_t>(count);
            std::transform(begin, end, begin, [vertexCount, i](const uint32_t vertexIndex) {
                return vertexIndex + (vertexCount * static_cast<uint32_t>(i));
            });
        }

        return indexData;
    }

    inline IndexData copyIndexData(const IndexData& indexData, const size_t instances = 1) {
        return IndexData{copyIndexData(indexData.data(), indexData.size(), instances)};
    }

    inline IndexData batchIndexData(const std::vector<IndexData>& indexDatas) {
        size_t batchedSize{};
        std::vector<uint32_t> vertexCounts(indexDatas.size());

        for (size_t i{}; i < vertexCounts.size(); i++) {
            const auto& indexData = indexDatas[i];
            vertexCounts[i] = *std::ranges::max_element(indexData) + 1;
            batchedSize += indexData.size();
        }

        IndexData batched(batchedSize);

        size_t offset{};
        uint32_t vertexIndexOffset{};
        for (size_t i{}; i < indexDatas.size(); i++) {
            const auto& indexData = indexDatas[i];
            const auto& offsetIter = batched.begin() + static_cast<ptrdiff_t>(offset);

            std::ranges::transform(indexData, offsetIter,
                                   [vertexIndexOffset](const uint32_t vertexIndex) {
                                       return vertexIndex + vertexIndexOffset;
                                   });

            offset += indexData.size();
            vertexIndexOffset += vertexCounts[i];
        }

        return batched;
    }
}
