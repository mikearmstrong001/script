#ifndef ER_HEADER_ASSERT_H
#define ER_HEADER_ASSERT_H


template <bool B>
struct CAssert {
};

template <>
struct CAssert<true> {
	static const int ok = 1;
};


#if 1
inline bool AssertReport(const char* condition, const char* msg, const char* file, int line) {
	return true;
}
#define ASSERT_HALT do { __debugbreak(); } while(__LINE__ == -1)

#define ASSERTS_ENABLED
#ifdef ASSERTS_ENABLED
	#undef ASSERT
	#define ASSERT(x) do { if (!(x) && AssertReport(#x, 0, __FILE__, __LINE__)) ASSERT_HALT; } while (__LINE__ == -1)
	#undef assert
	#define assert(x) do { if (!(x) && AssertReport(#x, 0, __FILE__, __LINE__)) ASSERT_HALT; } while (__LINE__ == -1)
#else
	#undef ASSERT
    #define ASSERT(x) \
        do { (void)sizeof(x); } while(0)
	#undef assert
    #define assert(x) \
        do { (void)sizeof(x); } while(0)
#endif

#define STATIC_ASSERT(x) \
	typedef char StaticAssert[(x) ? 1 : -1];
#else

extern void Error( const char *err, const char *msg, const char *file, int line );

#define assert(test) \
	if ( !(test) ) { \
		Error( #test, NULL, __FILE__, __LINE__ ); \
	}


#define ASSERT(test) \
	if ( !(test) ) { \
		Error( #test, NULL, __FILE__, __LINE__ ); \
	}

#define ASSERTMSG(test,msg) \
	if ( !(test) ) { \
		Error( #test, msg, __FILE__, __LINE__ ); \
	}

#define CTASSERT(condition) \
	extern char dummy_assert_array[(condition)?1:-1]

#endif
#endif
