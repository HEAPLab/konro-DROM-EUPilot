#ifndef MEMORYINFO_H
#define MEMORYINFO_H

namespace rmcommon {

/*!
 * Returns the total memory and the available memory in KB
 *
 * \param totalRamKb [out] total memory in KB
 * \param freeRamKb [out] free memory in KB
 */
void getMemoryInfo(unsigned long &totalRamKb, unsigned long &freeRamKb) noexcept;

/*!
 * Returns the total swap and the available swap in KB
 *
 * \param totalSwapKb [out] total swap in KB
 * \param freeSwapKb [out] free swap in KB
 */
void getSwapInfo(unsigned long &totalSwapKb, unsigned long &freeSwapKb) noexcept;

}   // namespace rmcommon

#endif // MEMORYINFO_H
