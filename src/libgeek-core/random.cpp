
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <geek/core-random.h>

using namespace Geek::Core;

Random::Random()
{
    init_urandom();
}

Random::Random(uint32_t s)
{
    seed(s);
}

bool Random::seed(uint32_t seed)
{
    int j;
    m_state[0] = seed & 0xffffffffUL;
    for (j = 1; j < MT_N; j++)
    {
        m_state[j] = (1812433253UL * (m_state[j-1] ^ (m_state[j-1] >> 30)) + j);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array state[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        m_state[j] &= 0xffffffffUL;  /* for >32 bit machines */
    }
    m_left = 1;
    m_initf = 1;
    m_initt = 1;
}

bool Random::init(uint32_t init_key[], int key_length)
{
    int i = 1;
    int j = 0;
    int k = (MT_N > key_length ? MT_N : key_length);

    seed(19650218UL);

    for (; k; k--)
    {
        m_state[i] =
           (m_state[i] ^ ((m_state[i-1] ^ (m_state[i-1] >> 30)) * 1664525UL));
        m_state[i] += init_key[j] + j; /* non linear */
        m_state[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        j++;
        if (i>=MT_N)
        {
            m_state[0] = m_state[MT_N-1];
            i=1;
        }
        if (j>=key_length)
            j=0;
    }

    for (k=MT_N-1; k; k--)
    {
        m_state[i] =
            (m_state[i] ^ ((m_state[i-1] ^ (m_state[i-1] >> 30)) * 1566083941UL));
        m_state[i] -= i; /* non linear */
        m_state[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i >= MT_N)
        {
            m_state[0] = m_state[MT_N-1];
            i=1;
        }
    }

    m_state[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
    m_left = 1;
    m_initf = 1;
}

bool Random::init_urandom()
{
    uint32_t entropy[MT_N];
    FILE* fd;
    int n = 0;

    fd = fopen("/dev/urandom", "r");
    if (!fd)
    {
        return false;
    }

while (n < MT_N)
{
uint32_t word;
int res;
res = fread(&word, 4, 1, fd);
if (res != 1)
{
break;
}
    entropy[n++] = word;
}
    fclose(fd);

if (n < MT_N)
{
    entropy[n++] = time(NULL);
}
if (n < MT_N)
{
    entropy[n++] = clock();
}

    if (n < MT_N) {
        struct timeval tv;
        if (gettimeofday(&tv, NULL) != -1) {
            entropy[n++] = tv.tv_usec;   /* Fractional part of current time */
        }
    }

    init(entropy, n);

    return true;
}

void Random::next_state()
{
    uint32_t *p = m_state;
    int j;

    m_left = MT_N;
    m_next = m_state;

    for (j = MT_N - MT_M + 1; --j; p++)
    {
        *p = p[MT_M] ^ TWIST(p[0], p[1]);
    }

    for (j=MT_M; --j; p++)
    {
        *p = p[MT_M-MT_N] ^ TWIST(p[0], p[1]);
    }

    *p = p[MT_M-MT_N] ^ TWIST(p[0], m_state[0]);
}

uint32_t Random::rand32()
{
    uint32_t y;
    if (--m_left == 0)
    {
        next_state();
    }
    y = *m_next++;

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    return (y ^ (y >> 18));
}

int Random::range(int min, int max)
{
    float r = (max - min) + 1;
    float v1 = ((float)rand32() / (float)(maxrand()));
    float v2 = v1 * r;
    return min + (int)v2;
}

double Random::ranged(double min, double max)
{
double rnd = (double)rand32() * (double)rand32();
    double r = (max - min) + 1;
    double v = (rnd / ((double)maxrand() * (double)maxrand() + 1)) * r;
    return min + v;
}



