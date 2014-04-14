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

#include <algorithm>
#include <stdexcept>

#include "matrix.h"
#include "matrix_test.h"


namespace am {
namespace test {


//-------------------------------------------------------------------
void matrix_initialization_correctness()
{
    matrix<int,4,3> m1 = {
        {11, 12, 13},
        {21, 22, 23},
        {31, 32, 33},
        {41, 42, 43}
    };

    matrix<int,1,3> m2 = {{1,2,3}};

    if(!(
        (m1(0,0) == 11) && (m1(0,1) == 12) && (m1(0,2) == 13) &&
        (m1(1,0) == 21) && (m1(1,1) == 22) && (m1(1,2) == 23) &&
        (m1(2,0) == 31) && (m1(2,1) == 32) && (m1(2,2) == 33) &&
        (m1(3,0) == 41) && (m1(3,1) == 42) && (m1(3,2) == 43) &&
        (m2(0,0) == 1) && (m2(0,1) == 2) && (m2(0,2) == 3) ))
    {
        throw std::logic_error("am::matrix initialization");
    }
}



//-------------------------------------------------------------------
void matrix_iterators_correctness()
{
    matrix<int,7,10> m;
    std::iota(begin(m), end(m), 11);

    long long int sum = 0;

    for(std::size_t j = 0; j < m.rows(); ++j) {
        for(auto i = m.begin_row(j), e = m.end_row(j); i != e; ++i) {
            sum += *i;
        }
    }
    sum *= 100;

    for(std::size_t j = 0; j < m.cols(); ++j) {
        for(auto i = m.begin_col(j), e = m.end_col(j); i != e; ++i) {
            sum += *i;
        }
    }
    sum *= 100;

    for(auto x : m.subrange(0,0, m.rows()-1, m.cols()-1)) {
        sum += x;
    }
    sum *= 100;

    for(std::size_t r = 0; r < m.rows(); ++r) {
        for(std::size_t c = 0; c < m.cols(); ++c) {
            for(auto x : m.subrange(0,0, r,c)) {
                sum += x;
            }
        }
    }

    for(int r =  m.rows()-1; r >= 0; --r) {
        for(int c = m.cols()-1; c >= 0; --c) {
            for(auto x : m.subrange(r,c, m.rows()-1, m.cols()-1)) {
                sum += x;
            }
        }
    }

    //diagonal iteration for square matrices
    {
        matrix<int,10,10> md;
        std::fill(begin(md), end(md), 0);
        std::fill(md.begin_diag(), md.end_diag(), 1);
        sum += std::accumulate(md.begin_diag(), md.end_diag(),0);
    }

    if(sum != 3217308650) {
        throw std::logic_error("am::matrix iteration");
    }
}



//-------------------------------------------------------------------
void matrix_correctness()
{
    matrix_initialization_correctness();
    matrix_iterators_correctness();
}



}  // namespace test
}  // namespace am


#endif
