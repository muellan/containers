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

#include "dynmatrix.h"


namespace am {
namespace test {


//-------------------------------------------------------------------
struct A {
	A(int y = 0) : x{y} {
		++n;
//		std::cout << "*";
	}
	A(const A& a) : x{a.x} {
		++n;
//		std::cout << "*";
	}

	~A() {
		--n;
//		std::cout << "+";
	}

	int x = 0;
	static int n;

	inline friend std::ostream& operator << (std::ostream& os, const A& a) {
		return (os << a.x);
	}

};

int A::n = 0;



//-------------------------------------------------------------------
bool dynmatrix_initialization_correct()
{
	dynmatrix<int> m1 = {
		{11, 12, 13},
		{21, 22, 23},
		{31, 32, 33},
		{41, 42, 43}
	};

	dynmatrix<int> m2 = {1,2,3};

	#ifdef AM_USE_EXCEPTIONS
		bool caught = false;

		try {
			dynmatrix<int> m3 = {{1,2}, {1,2,3}, {1,2}};
		}
		catch(dynmatrix_init_incoherent_row_sizes&) {
			caught = true;
		}
	#else
		bool caught = true;
	#endif

	return (
		caught &&
		(m1(0,0) == 11) && (m1(0,1) == 12) && (m1(0,2) == 13) &&
		(m1(1,0) == 21) && (m1(1,1) == 22) && (m1(1,2) == 23) &&
		(m1(2,0) == 31) && (m1(2,1) == 32) && (m1(2,2) == 33) &&
		(m1(3,0) == 41) && (m1(3,1) == 42) && (m1(3,2) == 43) &&
		(m2(0,0) == 1) && (m2(0,1) == 2) && (m2(0,2) == 3) );
}



//-------------------------------------------------------------------
bool dynmatrix_memory_correct()
{
	{
		dynmatrix<A> m;
		m.resize(1,2, {0});
		m.resize(6,6, {1});
		m.resize(2,3, {1});
		m.resize(10,10, {1});
		m.resize(2,2, {1});
		m.shrink_to_fit();
		m.resize(4,4, {1});
		m.erase_col(1);
		m.erase_row(1);
		m.reserve(20,20);
		m.resize(100,100, {1});

		for(int i = 0; i < 100; ++i)
			m.erase_col(0);

		m.resize(100,100, {1});
		for(int i = 0; i < 100; ++i)
			m.erase_row(0);

//		std::cout << m << std::endl;
	}

//	std::cout << "\n# " << A::n << '\n' << std::endl;
	return (A::n == 0);
}



//-------------------------------------------------------------------
bool dynmatrix_move_correct()
{
	int sum1 = 0, sum2 = 0, sum3 = 0;
	{
		dynmatrix<int> m;
		m.resize(9,9,0);
		int x = 10;
		std::generate(begin(m), end(m), [&]{++x; return x;});
		for(auto i : m) sum1 += i;

		dynmatrix<int> mm1 {std::move(m)};
		for(auto i : mm1) sum2 += i;

		m = std::move(mm1);
		for(auto i : m) sum3 += i;
	}

	return ((A::n == 0) && (sum1 == 4131) && (sum2 == 4131) && (sum3 == 4131));
}



//-------------------------------------------------------------------
bool dynmatrix_resizing_correct()
{
	dynmatrix<int> m;
	m.rows(10, 3);
	m.cols(4, 3);

	std::iota(begin(m), end(m), 11);

	m.insert_cols(4,1,9);

	m.resize(7,8,88);
	m.cols(7,8);

	for(size_t i = 0; i < 5; ++i) {
		m.erase_col(1);
	}

	for(size_t i = 0; i < 5; ++i) {
		m.erase_row(1);
	}

	m.fill_row(2,3);
	m.fill_row(3,4);
	m.fill_row(4,6);

	m.insert_rows(2,2,2);
	m.insert_rows(2,1,2);
	m.insert_rows(5,1,3);
	m.insert_rows(8,1,4);
	m.insert_rows(9,2,5);
	m.insert_rows(12,1,7);

	m.erase_row(1);
	m.erase_rows(1,2);
	m.erase_row(3);
	m.erase_row(3);
	m.erase_row(4);
	m.shrink_to_fit();

	return (
		(m(0,0) == 11) && (m(0,1) == 88) &&
		(m(1,0) == 2) && (m(1,1) == 2) &&
		(m(2,0) == 3) && (m(2,1) == 3) &&
		(m(3,0) == 0) && (m(3,1) == 0) );
}



//-------------------------------------------------------------------
bool dynmatrix_iterators_correct()
{
	dynmatrix<int> m;
	m.insert_cols(0,10);
	m.insert_rows(0,6);

	int x = 10;
	std::generate(begin(m), end(m), [&]{++x; return x;});

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

	auto i = m.traverse_block(0,0, m.rows()-1, m.cols()-1);
	while(i) {
		sum += *i;
		++i;
	}
	sum *= 100;

	for(std::size_t r = 0; r < m.rows(); ++r) {
		for(std::size_t c = 0; c < m.cols(); ++c) {
			i = m.traverse_block(0,0, r,c);
			while(i) {
				sum += *i;
				++i;
			}
		}
	}

	for(int r =  m.rows()-1; r >= 0; --r) {
		for(int c = m.cols()-1; c >= 0; --c) {
			i = m.traverse_block(r,c, m.rows()-1, m.cols()-1);
			while(i) {
				sum += *i;
				++i;
			}
		}
	}

	return (sum == 3217308640);
}



//-------------------------------------------------------------------
bool dynmatrix_correct()
{
	return (
			dynmatrix_initialization_correct() &&
			dynmatrix_memory_correct() &&
			dynmatrix_move_correct() &&
			dynmatrix_resizing_correct() &&
			dynmatrix_iterators_correct()
		);
}


}  // namespace test
}  // namespace am


#endif
