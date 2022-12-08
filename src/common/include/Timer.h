#include <chrono> // for std::chrono functions

class Timer
{
private:
    // Type aliases to make accessing nested type easier
    using Clock = std::chrono::steady_clock;
    std::chrono::time_point<Clock> m_beg { Clock::now() };

public:
    void reset() { m_beg = Clock::now(); }

    double elapsed() const {
        return std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - m_beg).count();
    }

    double elapsedMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - m_beg).count();
    }
};
