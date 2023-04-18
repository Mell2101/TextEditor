#pragma once

#include <stdio.h>

#define ASSERT(expression) \
    if (!(expression))\
    {\
        printf("%s [line %d] in '%s' -- FAILED!!!\n",__PRETTY_FUNCTION__, __LINE__, __FILE__);\
    }

