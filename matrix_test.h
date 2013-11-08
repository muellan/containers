/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_CONTAINERS_MATRIX_TEST_H_
#define AM_CONTAINERS_MATRIX_TEST_H_

#ifdef AM_USE_TESTS


namespace am {
namespace test {


bool matrix_initialization_correct();
bool matrix_iterators_correct();

bool matrix_correct();


}  // namespace test
}  // namespace am


#endif

#endif
