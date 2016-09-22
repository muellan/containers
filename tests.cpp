/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2015 André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include <iostream>

#include "tests.h"



//-------------------------------------------------------------------
void container_correctness()
{
    test::matrix_array_correctness();
    test::dynamic_matrix_correctness();
    test::triangle_matrix_correctness();
    test::crs_matrix_correctness();
    test::crs_matrix_correctness();
    test::compressed_multiset_correctness();
}



//-------------------------------------------------------------------
int main()
{
    container_correctness();
}


#endif
