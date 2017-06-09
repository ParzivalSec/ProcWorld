#pragma once
#include <chrono>

/**
* Namespace containing several usefule random number generators
*/
namespace random
{
	/*
	* XORShift random number generator
	*/
	namespace xorshift
	{
		namespace
		{
			static unsigned long x = static_cast<unsigned long>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
			static unsigned long y = 0xC0FFEE;
			static unsigned long z = 0xDEADBEEF;
			static unsigned long t = 0;
		}

		static unsigned long rnd(void)
		{
			x ^= x << 16;
			x ^= x >> 5;
			x ^= x << 1;

			t = x;
			x = y;
			y = z;
			z = t ^ x ^ y;

			return z;
		}

		static int IntRange(int min, int max)
		{
			return min + (xorshift::rnd() % static_cast<int>(max - min + 1));
		}
	}

	/**
	* Mother of All random number generator
	*/
	namespace mother
	{
		// TODO: Implement mother-of-all
	}
}
