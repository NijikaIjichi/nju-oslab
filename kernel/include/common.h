#ifndef __COMMON_H__
#define __COMMON_H__

#include "common/types.h"
#include "common/const.h"
#include "common/assert.h"

#define LENGTH(a) (sizeof(a)/sizeof((a)[0]))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

#endif
