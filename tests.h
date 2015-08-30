/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2015 André Müller
 *
 *****************************************************************************/

#ifndef AM_CONTAINERS_TESTS_H_
#define AM_CONTAINERS_TESTS_H_

#ifdef AM_USE_TESTS

#include "dynamic_matrix_test.h"
#include "matrix_array_test.h"
#include "triangle_matrix_test.h"
#include "crs_matrix_test.h"


namespace am {
namespace test {


//-------------------------------------------------------------------
void container_correctness();


}  // namespace test
}  // namespace am


#endif
#endif
