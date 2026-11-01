#ifndef OCP_DOS_THREADING_H
#define OCP_DOS_THREADING_H

#include <mutex>
#include <thread>
#include <cmath>
#include <stddef.h>

extern "C"
{
int strcasecmp(const char *, const char *);
int strncasecmp(const char *, const char *, size_t);
}

namespace std
{
class mutex
{
public:
	void lock() noexcept {}
	void unlock() noexcept {}
};

class dos_thread
{
public:
	template <typename Callable>
	explicit dos_thread(Callable &&callable) { callable(); }
	void join() noexcept {}
};

inline double log1p(double value) noexcept
{
	return log(1.0 + value);
}
}

#define thread dos_thread
#define jthread dos_thread

#endif
