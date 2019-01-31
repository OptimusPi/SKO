#include <chrono>
#include <thread>

class OPI_Sleep
{
public:
    static void nanoseconds(unsigned int time)
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(time));
    }

    static void microseconds(unsigned int time)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(time));
    }

    static void milliseconds(unsigned int time)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }

    static void seconds(unsigned int time)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(time));
    }
};