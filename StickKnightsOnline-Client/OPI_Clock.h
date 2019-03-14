#include <chrono>
using namespace std::chrono;

class OPI_Clock
{
public:
    static unsigned long long nanoseconds()
    {
        return time_point_cast<std::chrono::nanoseconds>(system_clock::now())
            .time_since_epoch().count();
    }
    static unsigned long long microseconds()
    {
        return time_point_cast<std::chrono::microseconds>(system_clock::now())
            .time_since_epoch().count();
    }
    static unsigned long long milliseconds()
    {
        return time_point_cast<std::chrono::milliseconds>(system_clock::now())
            .time_since_epoch().count();
    }
    static unsigned long long seconds()
    {
        return time_point_cast<std::chrono::seconds>(system_clock::now())
            .time_since_epoch().count();
    }
};
