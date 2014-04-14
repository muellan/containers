/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifndef AM_CONTAINERS_TESTS_H_
#define AM_CONTAINERS_TESTS_H_

#ifdef AM_USE_TESTS

#include "dynmatrix_test.h"
#include "matrix_test.h"
#include "pairmap_test.h"
#include "dynpairmap_test.h"
#include "sparse_pairmap_test.h"


namespace am {
namespace test {


//-------------------------------------------------------------------
void container_correctness();


}  // namespace test
}  // namespace am


#endif
#endif
