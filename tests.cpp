/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include <iostream>

#include "tests.h"


namespace am {
namespace test {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
void container_correctness()
{
    try {
        matrix_correctness();
    }
    catch(std::exception& e) {
        std::cerr << "FAIL\n" << "am::matrix\n" << e.what() << std::endl;
    }

    try {
        dynmatrix_correctness();
    }
    catch(std::exception& e) {
        std::cerr << "FAIL\n" << "am::dynmatrix\n" << e.what() << std::endl;
    }

}



}  // namespace test
}  // namespace am


#endif
