#ifdef NDEBUG
#define DEBUG(x)
#else
#define DEBUG(X) X
#endif

/* REPLACE WITH EXIT */
#define EXIT_IF(x, r)            \
	{                            \
		if ((x))                 \
		{                        \
			printf("EXITING\n"); \
		}                        \
	}

#define LOCK(m, x)                    \
	{                                 \
		pthread_mutex_lock(&m);       \
		x                             \
			pthread_mutex_unlock(&m); \
	}

#define RET_IF(x, r)    \
	{                   \
		if ((x))        \
		{               \
			return (r); \
		}               \
	}

#if __cplusplus < 201103L
#define nullptr 0
#define override
#define noexcept throw()
#endif
