#ifndef CORE_ASSERT_H
#define CORE_ASSERT_H

#include <assert.h>

#define ASSERT( COND, MSG ) \
    assert((COND) && MSG);


#endif // CORE_ASSERT_H