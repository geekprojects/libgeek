/*
 * libgeek - The GeekProjects utility suite
 * Copyright (C) 2014, 2015, 2016 GeekProjects.com
 *
 * This file is part of libgeek.
 *
 * libgeek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libgeek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgeek.  If not, see <http://www.gnu.org/licenses/>.
 */

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

template<typename _Value> class DynamicArray;

template<typename _Value> class DynamicArrayIterator
{
 private:
    DynamicArray<_Value>* m_array;
    int m_position;

 public:
    DynamicArrayIterator()
    {
        m_array = NULL;
    }

    DynamicArrayIterator(DynamicArray<_Value>* array)
    {
        m_array = array;
        m_position = m_array->getMinIndex();
    }

    DynamicArrayIterator(DynamicArray<_Value>* array, int pos)
    {
        m_array = array;
        m_position = pos;
    }

    bool operator != (DynamicArrayIterator<_Value> rhs)
    {
        return
            this->m_array != rhs.m_array ||
            this->m_position != rhs.m_position;
    }

    DynamicArrayIterator<_Value>& operator ++(int)
    {
        while (!m_array->hasValue(++m_position))
        {
            if (m_position > m_array->getMaxIndex())
            {
                m_position = -1;
                break;
            }
        }
        return *this;
    }

    int getIndex()
    {
        return m_position;
    }

    _Value& operator *()
    {
        return (*m_array)[m_position];
    }
};

template<typename _Value> class DynamicArray
{
 public:
    typedef DynamicArrayIterator<_Value> iterator;

 private:
    _Value* m_array;
    int m_capacity;
    int m_arrayStart;
    int m_minIndex;
    int m_maxIndex;
    _Value m_default;

 public:

    /* Note, this constructor will only work with pointer types! */
    DynamicArray()
    {
        m_array = NULL;
        m_capacity = 0;
        m_arrayStart = 0;
        m_minIndex = 0;
        m_maxIndex = 0;
        m_default = NULL;
    }

    DynamicArray(const _Value def)
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

    _Value& operator[](int index)
    {
        if (m_array == NULL || index < m_arrayStart || index > m_maxIndex)
        {
            // Definitely not in the array. Create an entry with the default
            insert(index, m_default);
        }
        return m_array[index - m_arrayStart];
    }

    bool isEmpty()
    {
        return m_capacity == 0;
    }

    bool hasValue(int index)
    {
        if (m_array == NULL || index < m_arrayStart || index > m_maxIndex)
        {
            return false;
        }
        return m_array[index - m_arrayStart] != m_default;
    }

    int getMinIndex()
    {
        return m_minIndex;
    }

    int getMaxIndex()
    {
        return m_maxIndex;
    }

    iterator begin()
    {
        return DynamicArray<_Value>::iterator(this);
    }

    iterator end()
    {
        return DynamicArray<_Value>::iterator(this, -1);
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

 public:
};

};
};

#endif
