#ifndef LOGDEF_H
#define LOGDEF_H

#include <string>
#include "esp_log.h"
#include <string.h>


template <typename T, size_t S>

inline constexpr size_t get_file_name_offset(const T (& str)[S], size_t i = S - 1)
{
    return (str[i] == '/' || str[i] == '\\') ? i + 1 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
}

template <typename T>
inline constexpr size_t get_file_name_offset(T (& str)[1])
{
    return 0;
}

#define FNAME &__FILE__[get_file_name_offset(__FILE__)]

#endif
