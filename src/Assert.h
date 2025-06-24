#ifndef ASSERT_H
#define ASSERT_H

#if defined(_MSC_VER)
#define debugBreak() (__debugbreak())
#elif defined(__clang__) || defined(__GNUC__)
#define debugBreak() (raise(SIGTRAP))
#else
#define debugBreak() (assert(false))
#endif

inline bool debugAssert(const bool condition, const std::string& message = {}) {
    if (!condition) {
        std::cerr << message;
        debugBreak();
    }

    return condition;
}

#ifdef NDEBUG
#define ASSERT(condition) ()
#define ASSERT_MSG(condition, message) ()
#else
#define ASSERT(condition) debugAssert(condition)
#define ASSERT_MSG(condition, message) debugAssert(condition, message)
#endif

#endif
