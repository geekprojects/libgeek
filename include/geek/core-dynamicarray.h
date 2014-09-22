#ifndef __LIBGEEK_CORE_DYNAMICARRAY_H_
#define __LIBGEEK_CORE_DYNAMICARRAY_H_

#include <string.h>

#ifdef __DEBUG_DYNAMICARRAY
#include <stdio.h>
#endif

namespace Geek
{
namespace Core
{

template<typename _Value> class DynamicArray
{
    _Value* m_array;
    int m_capacity;
    int m_arrayStart;
    int m_minIndex;
    int m_maxIndex;
    _Value m_default;

 public:
    DynamicArray(_Value def)
    {
        m_array = NULL;
        m_capacity = 0;
        m_arrayStart = 0;
        m_minIndex = 0;
        m_maxIndex = 0;
        m_default = def;
    }

    virtual ~DynamicArray()
    {
        if (m_array != NULL)
        {
            delete[] m_array;
        }
    }

    void insert(int i, _Value value)
    {
        if (m_array == NULL)
        {
            // First insertion. Use it to base the arrayStart
            m_capacity = 10;
            m_array = alloc(m_capacity);
            m_arrayStart = i;
            m_minIndex = i;
            m_maxIndex = i;
        }
        else
        {
            if (i < m_arrayStart)
            {
                int initialCapacity = m_capacity;
                int diff = m_arrayStart - i;

                // Allocate extra capacity equivilant to double the difference
                // to the current array start
                int allocExtent = diff * 2;

                m_capacity += allocExtent;
                _Value* newArray = alloc(m_capacity);
                memcpy(
                    newArray + (allocExtent),
                    m_array,
                    sizeof(_Value) * initialCapacity);
                m_arrayStart -= allocExtent;
                delete[] m_array;
                m_array = newArray;
            }
            else if ((i - m_arrayStart) >= m_capacity)
            {
                // More capacity is required, double it.
                int initialCapacity = m_capacity;
                int diff = (i - m_arrayStart) - m_capacity;
                int allocExtra = diff * 2;
                if (allocExtra < m_capacity)
                {
                    allocExtra = m_capacity;
                }
                m_capacity += allocExtra;
                _Value* newArray = alloc(m_capacity);
                memcpy(newArray, m_array, sizeof(_Value) * initialCapacity);
                delete[] m_array;
                m_array = newArray;
            }
        }

        if (i < m_minIndex)
        {
            m_minIndex = i;
        }
        if (i > m_maxIndex)
        {
            m_maxIndex = i;
        }

        m_array[i - m_arrayStart] = value;
    }

    _Value operator[](int index) const
    {
        if (index < m_arrayStart || index > m_maxIndex)
        {
            // Definitely not in the array. Return the default
            return m_default;
        }
        return m_array[index - m_arrayStart];
    }

    bool isEmpty()
    {
        return m_capacity == 0;
    }

    int getMinIndex()
    {
        return m_minIndex;
    }

    int getMaxIndex()
    {
        return m_maxIndex;
    }

#ifdef __DEBUG_DYNAMICARRAY
    void dump()
    {
        printf("DynamicArray::dump: arrayStart=%d, capacity=%d\n", m_arrayStart, m_capacity);
        int i;
        for (i = 0; i < m_capacity; i++)
        {
            printf("DynamicArray::dump: [%d => %d] = %0.2f\n", i, i + m_arrayStart, m_array[i]);
        }
    }
#endif

 private:
    _Value* alloc(int size)
    {
        _Value* arr = new _Value[size];
        int i;
        for (i = 0; i < size; i++)
        {
            arr[i] = m_default;
        }
        return arr;
    }
};

};
};

#endif
