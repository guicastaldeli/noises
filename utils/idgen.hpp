/******************************* 
 * 
 * Id generator... 
 * 
 ********************************/
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <mutex>
#include <random>
#include <stdexcept>

class IdGen {
private:
    struct Stream {
        uint64_t next = 1;
    };

    inline static std::mutex mutex_;
    inline static uint64_t seed_ = 0;
    inline static std::unordered_map<std::string, Stream> streams_;

    static void ensureSeeded() {
        std::lock_guard<std::mutex> lock(mutex_);
        if(seed_ == 0) {
            std::random_device rd;
            seed_ = (uint64_t(rd()) << 32) ^ rd();
        }
    }

public:
    IdGen() = delete;
    ~IdGen() = delete;

    /**
     * Next
     */
    static uint64_t next(const std::string& stream) {
        ensureSeeded();

        std::lock_guard<std::mutex> lock(mutex_);
        Stream& s = streams_[stream];
        if(s.next == 0) {
            throw std::runtime_error("IdGen: id space exhausted for stream '" + stream + "'");
        }

        return s.next++;
    }

    /**
     * Peek
     */
    static uint64_t peek(const std::string& stream) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = streams_.find(stream);
        return it == streams_.end() ? 1 : it->second.next;
    }

    /**
     * Reset
     */
    static void reset(const std::string& stream = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        if(stream.empty()) {
            streams_.clear();
        } else {
            streams_[stream] = Stream{};
        }
    }

    /**
     * Count
     */
    static uint64_t count(const std::string& stream) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = streams_.find(stream);
        return it == streams_.end() ? 0 : (it->second.next - 1);
    }
};