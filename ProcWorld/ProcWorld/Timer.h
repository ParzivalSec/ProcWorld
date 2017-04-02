#ifndef _TIMER_H_
#define _TIMER_H_
#include <chrono>

using namespace std::chrono;

/**
* class Timer
* Utility class to measure the elapsed
* time between two time-points. The points
* are set via the Start() and Stop() methods
*/
class Timer
{
public:

	high_resolution_clock::time_point& StartTimer(void);
	high_resolution_clock::time_point& StopTimer(void);
	duration<float>& GetElapsedTime(void);

	void PrintElapsedTime(void) const;

private:
	high_resolution_clock::time_point m_startPoint, m_stopPoint;
	mutable duration<float> m_elapsedTime;
};

#endif // _TIMER_H_