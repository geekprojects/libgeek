
#include <geek/core-dynamicarray.h>

#include <stdio.h>
#include <stdint.h>
#include <math.h>

using namespace Geek::Core;

int main(int argc, char** argv)
{
    DynamicArray<float> array(NAN);

    uint64_t checksum = 0;

    int i;
    // Test inserting values
    for (i = 0; i < 10; i++)
    {
        int index = (i * 2) + 100;
        array.insert(index, (float)i);
        checksum += (index * 1318699) + (i * 564);
    }

    // Test inserting values that have indexes lower than the current values
    for (i = 0; i < 5; i++)
    {
        int index = 20 - (i * 2);
        array.insert(index, (float)i);
        checksum += (index * 1318699) + (i * 564);
    }

    // Test setting values via the [] operator
    for (i = 0; i < 10; i++)
    {
        int index = (i * 2) + 500;
        array[index] = (float)i;
        checksum += (index * 1318699) + (i * 564);
    }

    // Test modifying the content via the [] operator
    for (i = array.getMinIndex(); i <= array.getMaxIndex(); i++)
    {
        if (!isnan(array[i]))
        {
            array[i] += i;
            checksum += i * 564;
        }
    }

    uint64_t readChecksum = 0;
    // Test iterators
    DynamicArray<float>::iterator it;
    for (it = array.begin(); it != array.end(); it++)
    {
printf("dynamicarray iterator: %d = %0.2f\n", it.getIndex(), *it);
        if (!isnan(*it))
        {
            readChecksum += (it.getIndex() * 1318699) + (((int)floor(*it)) * 564);
        }
    }

    if (checksum == readChecksum)
    {
        printf("dynamicarray iterator test: PASS\n");
    }
    else
    {
        printf("dynamicarray iterator test: FAIL %ld != %ld\n", checksum, readChecksum);
    }


#if 0
    array.dump();
#endif

    readChecksum = 0;
    for (i = array.getMinIndex(); i <= array.getMaxIndex(); i++)
    {
        float value = array[i];
        if (!isnan(value))
        {
#if 0
            printf("dynamicarray test: %d = %0.2f\n", i, value);
#endif
            readChecksum += (i * 1318699) + (((int)floor(value)) * 564);
        }
    }

    if (checksum == readChecksum)
    {
        printf("dynamicarray min/max test: PASS\n");
    }
    else
    {
        printf("dynamicarray min/max test: FAIL %ld != %ld\n", checksum, readChecksum);
    }

    return 0;
}

