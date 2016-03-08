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

#ifndef __LIBGEEK_CORE_COMPILER_H_
#define __LIBGEEK_CORE_COMPILER_H_

#if !defined(__clang__) && defined(__GNUC__)

   /*
    * This macro should simplify gcc version checking. For example, to check
    * for gcc 4.5.1 or later, check `#ifdef MOZ_GCC_VERSION_AT_LEAST(4, 5, 1)`.
    */
#define GEEK_GCC_VERSION_AT_LEAST(major, minor, patchlevel)          \
     ((__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) \
      >= ((major) * 10000 + (minor) * 100 + (patchlevel)))

#else

#define GEEK_GCC_VERSION_AT_LEAST(major, minor, patchlevel) 0

#endif

#if GEEK_GCC_VERSION_AT_LEAST(4, 7, 0)
#define GEEK_SUPPORTS_VECTOR_MATHS 1
#else
#undef GEEK_SUPPORTS_VECTOR_MATHS
#endif

#endif
