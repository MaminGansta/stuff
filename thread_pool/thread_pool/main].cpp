#include <cstdio>

#include "thread_pool.cpp"

int main(void)
{
	thread_pool pool;

	auto res = pool.add_task([]() {return 1; });

	printf("%d", res.get());

	return 0;
}