#ifndef MEMORYINFO_H
#define MEMORYINFO_H

namespace rmcommon {

/*!
 * Returns the total amount of RAM on the machine in KB
 *
 * \param totalRamKb [out] total RAM in KB
 */
void getMemoryInfo(unsigned long &totalRamKb) noexcept;

/*!
 * Returns the total amount of SWAP on the machine in KB
 *
 * \param totalSwapKb [out] total SWAP in KB
 */
void getSwapInfo(unsigned long &totalSwapKb) noexcept;

}   // namespace rmcommon

#endif // MEMORYINFO_H
