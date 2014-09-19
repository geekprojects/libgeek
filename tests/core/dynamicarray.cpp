
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
    for (i = 0; i < 10; i++)
    {
        int index = (i * 2) + 100;
        array.insert(index, (float)i);
        checksum += (index * 1318699) + (i * 564);
    }

    for (i = 0; i < 5; i++)
    {
        int index = 20 - (i * 2);
        array.insert(index, (float)i);
        checksum += (index * 1318699) + (i * 564);
    }

    for (i = 0; i < 10; i++)
    {
        int index = (i * 2) + 500;
        array.insert(index, (float)i);
        checksum += (index * 1318699) + (i * 564);
    }

#if 0
    array.dump();
#endif

    uint64_t readChecksum = 0;
    for (i = 0; i < 1000; i++)
    {
        float value = array[i];
        if (!isnan(value))
        {
            printf("dynamicarray test: %d = %0.2f\n", i, value);
            readChecksum += (i * 1318699) + (((int)floor(value)) * 564);
        }
    }

    if (checksum == readChecksum)
    {
        printf("dynamicarray test: PASS\n");
    }
    else
    {
        printf("dynamicarray test: FAIL\n");
    }

    return 0;
}

