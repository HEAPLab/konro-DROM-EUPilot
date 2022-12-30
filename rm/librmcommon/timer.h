#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

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
    explicit Timer(bool reset = false) {
        if (reset)
            Reset();
    }

    /*!
     * Stores the current time point
     */
    void Reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }

    /*!
     * Returns the duration between the current time point and the
     * time point stored by the last Reset() call.
     *
     * \param reset if true, the current time point becomes the new
     *              starting point for duration calculation
     */
    T Elapsed(bool reset = false) {
        std::chrono::high_resolution_clock::time_point cur = std::chrono::high_resolution_clock::now();
        T val = std::chrono::duration_cast<T>(cur - start_);
        if (reset)
            Reset();
        return val;
    }

    template <typename U, typename Traits>
    friend std::basic_ostream<U, Traits>& operator<<(std::basic_ostream<U, Traits>& out,
                                                     const Timer& timer) {
        return out << timer.Elapsed().count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
};

}   //  namespace rmcommon

#endif // TIMER_H
