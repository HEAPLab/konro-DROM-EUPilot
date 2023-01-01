#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>
#include <ctime>

namespace rmcommon {

/*!
 * A simple class for elapsed time measurement.
 *
 * T std::chrono::nanoseconds, ...,  std::chrono::minutes
 *
 * \code
 *  Timer<std::chrono::microseconds> t;
 *
 *  t.Reset()
 *  ... code
 *  std::chrono::microseconds us = t.Elapsed();
 *  std::cout << us.count() << " microseconds elapsed\n";
 * \endcode
 */
template<typename T>
class Timer {
public:
    enum TimerMode {
        TIMER_CONTINUE,     // continue with the same initial time point
        TIMER_RESTART       // reset initial time point ot "now"
    };

    explicit Timer() {
        Restart();
    }

    /*!
     * Stores the current time point as starting point for
     * elapsed time measurement
     */
    void Restart() {
        start_ = std::chrono::high_resolution_clock::now();
    }

    /*!
     * Returns the duration between the current time point and the
     * time point stored by the last Reset() call.
     *
     * \param mode if TIMER_RESET, the current time point becomes the new
     *             starting point for duration calculation (i.e. a new
     *             measurement starts
     */
    T Elapsed(TimerMode mode = TIMER_CONTINUE) {
        std::chrono::high_resolution_clock::time_point cur = std::chrono::high_resolution_clock::now();
        T val = std::chrono::duration_cast<T>(cur - start_);
        if (mode == TIMER_RESTART)
            Restart();
        return val;
    }

    template <typename U, typename Traits>
    friend std::basic_ostream<U, Traits>& operator<<(std::basic_ostream<U, Traits>& out,
                                                     const Timer& timer) {
        return out << timer.Elapsed().count();
    }

    static long getSystemMicroseconds() {
        struct timespec ts = {};
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000000L + ts.tv_nsec / 1000L;
    }
private:
    std::chrono::high_resolution_clock::time_point start_;
};

}   //  namespace rmcommon

#endif // TIMER_H
