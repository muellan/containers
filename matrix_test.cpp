/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include <algorithm>

#include "matrix.h"


namespace am {
namespace test {


//-------------------------------------------------------------------
bool matrix_initialization_correct()
{
	matrix<int,4,3> m1 = {
		{11, 12, 13},
		{21, 22, 23},
		{31, 32, 33},
		{41, 42, 43}
	};

	matrix<int,1,3> m2 = {{1,2,3}};

	return (
		(m1(0,0) == 11) && (m1(0,1) == 12) && (m1(0,2) == 13) &&
		(m1(1,0) == 21) && (m1(1,1) == 22) && (m1(1,2) == 23) &&
		(m1(2,0) == 31) && (m1(2,1) == 32) && (m1(2,2) == 33) &&
		(m1(3,0) == 41) && (m1(3,1) == 42) && (m1(3,2) == 43) &&
		(m2(0,0) == 1) && (m2(0,1) == 2) && (m2(0,2) == 3) );
}



//-------------------------------------------------------------------
bool matrix_iterators_correct()
{
	matrix<int,7,10> m;
	std::iota(begin(m), end(m), 11);

	long long int sum = 0;

	for(std::size_t j = 0; j < m.rows(); ++j) {
		for(auto i = m.row_begin(j), e = m.row_end(j); i != e; ++i) {
			sum += *i;
		}
	}
	sum *= 100;

	for(std::size_t j = 0; j < m.cols(); ++j) {
		for(auto i = m.col_begin(j), e = m.col_end(j); i != e; ++i) {
			sum += *i;
		}
	}
	sum *= 100;

	auto i = m.block_iter(0,0, m.rows()-1, m.cols()-1);
	while(i) {
		sum += *i;
		++i;
	}
	sum *= 100;

	for(std::size_t r = 0; r < m.rows(); ++r) {
		for(std::size_t c = 0; c < m.cols(); ++c) {
			i = m.block_iter(0,0, r,c);
			while(i) {
				sum += *i;
				++i;
			}
		}
	}

	for(int r =  m.rows()-1; r >= 0; --r) {
		for(int c = m.cols()-1; c >= 0; --c) {
			i = m.block_iter(r,c, m.rows()-1, m.cols()-1);
			while(i) {
				sum += *i;
				++i;
			}
		}
	}

	//diagonal iteration for square matrices
	{
		matrix<int,10,10> md;
		std::fill(begin(md), end(md), 0);
		std::fill(md.diag_begin(), md.diag_end(), 1);
		sum += std::accumulate(md.diag_begin(), md.diag_end(),0);
	}

	return (sum == 3217308650);
}



//-------------------------------------------------------------------
bool matrix_correct()
{
	return (
			matrix_initialization_correct()	&&
			matrix_iterators_correct()
		);
}



}  // namespace test
}  // namespace am


#endif
