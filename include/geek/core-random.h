#ifndef __GEEK_RANDOM_H_
#define __GEEK_RANDOM_H_

#include <stdint.h>

namespace Geek
{
namespace Core
{

#define MT_N 624
#define MT_M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UMASK 0x80000000UL /* most significant w-r bits */
#define LMASK 0x7fffffffUL /* least significant r bits */
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))


class Random
{
 private:
    uint32_t m_state[MT_N];
    uint32_t* m_next;
    int m_left;
    int m_initf;
    int m_initt;

    bool seed(uint32_t s);
    bool init(uint32_t key[], int length);
    bool init_urandom();
    void next_state();

 public:
    Random();
    Random(uint32_t seed);

    uint32_t rand32();
    uint32_t maxrand() { return 4294967295UL; } // unsigned 32 bits
    int range(int min, int max);
    double ranged(double min, double max);
};

}
}

#endif
