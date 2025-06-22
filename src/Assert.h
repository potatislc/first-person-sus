#ifndef ASSERT_H
#define ASSERT_H

#if defined(_MSC_VER)
#define debugBreak() (__debugbreak())
#elif defined(__clang__) || defined(__GNUC__)
#define debugBreak() (raise(SIGTRAP))
#else
#define debugBreak() (assert(false))
#endif

inline bool debugAssert(const bool condition) {
    if (!condition) {
        debugBreak();
    }

    return condition;
}

#ifdef NDEBUG
#define ASSERT(condition) ()
#else
#define ASSERT(condition) debugAssert(condition)
#endif

#endif
