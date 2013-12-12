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
#include <numeric>
#include <iostream>

#include "sparse_pairmap.h"


namespace am {
namespace test {


//-------------------------------------------------------------------
bool sparse_pairmap_correct()
{
	sparse_pairmap<int> pm;

	const std::size_t n = 9;

	for(size_t i = 0; i < n; i += 2) {
		for(size_t j = i+2; j < n; j += 2) {
			pm(i,j) = 10*(i+1) + j+1;
		}
	}

//	for(size_t f = 0; f < n; ++f) {
//		for(size_t l = f; l < n; ++l) {
//			auto r = pm.subrange(f,l);

//			std::cout << "[" << f << "," << l << "] : " << content(r) << std::endl;

//			std::cout << "[" << f << "," << l << "] : "
//				<< std::accumulate(r.begin(), r.end(), 0)
//				<< " == " << rr[{f,l}]
//				<< std::endl;

//			if(std::accumulate(r.begin(), r.end(), 0) != rr[{f,l}])
//				return false;
//		}
//	}


	pm(0,9) = 1000;
	pm(1,10) = 1000;
	pm(2,9) = 1000;
	pm(2,10) = 1000;
	pm(2,11) = 1000;
	pm(4,20) = 1000;
	pm(9,10) = 1000;
	pm(9,20) = 1000;
	pm(10,20) = 1000;
	pm(10,11) = 1000;
	pm(11,20) = 1000;

	auto pm2 = std::move(pm);
	auto pm1 = sparse_pairmap<int>{pm2};

	auto pm3 = sparse_pairmap<int>();
	pm3(0,1) = 0;
	pm3(0,3) = 1;
	pm3(0,5) = 2;
	pm3(0,7) = 3;
	pm3(1,2) = 4;
	pm3(1,4) = 5;
	pm3(1,6) = 6;
	pm3(2,3) = 7;
	pm3(2,4) = 8;
	pm3(3,4) = 9;
	pm3(3,7) = 10;
	pm3(4,6) = 11;
	pm3(4,7) = 12;
	pm3(5,7) = 13;
	pm3(6,7) = 14;

	pm1.erase_index(20);
	pm1.erase_range(9,11);

	pm3.swap_indices(0,3);
	pm3.swap_indices(2,4);

	return (true
		&& (std::accumulate(pm1.begin(0), pm1.end(0), 0) == 64)
		&& (std::accumulate(pm1.begin(2), pm1.end(2), 0) == 124)
		&& (std::accumulate(pm1.begin(4), pm1.end(4), 0) == 166)
		&& (std::accumulate(pm1.begin(6), pm1.end(6), 0) == 190)
		&& (std::accumulate(pm1.begin(8), pm1.end(8), 0) == 196)
		&& (std::accumulate(pm3.begin(0), pm3.end(0), 0) == 27)
		&& (std::accumulate(pm3.begin(1), pm3.end(1), 0) == 15)
		&& (std::accumulate(pm3.begin(2), pm3.end(2), 0) == 45)
		&& (std::accumulate(pm3.begin(3), pm3.end(3), 0) == 6)
		&& (std::accumulate(pm3.begin(4), pm3.end(4), 0) == 19)
		&& (std::accumulate(pm3.begin(5), pm3.end(5), 0) == 15)
		&& (std::accumulate(pm3.begin(6), pm3.end(6), 0) == 31)
		&& (std::accumulate(pm3.begin(7), pm3.end(7), 0) == 52)
		&& !pm.contains(0,1) && !pm.contains(1,0)
		&& !pm.contains(0,3) && !pm.contains(3,0)
		&& !pm.contains(0,5) && !pm.contains(5,0)
		&& !pm.contains(0,7) && !pm.contains(7,0)
		&& !pm.contains(1,2) && !pm.contains(2,1)
		&& !pm.contains(1,3) && !pm.contains(3,1)
		&& !pm.contains(1,4) && !pm.contains(4,1)
		&& !pm.contains(1,5) && !pm.contains(5,1)
		&& !pm.contains(1,6) && !pm.contains(6,1)
		&& !pm.contains(1,7) && !pm.contains(7,1)
		&& !pm.contains(1,8) && !pm.contains(8,1)
		&& !pm.contains(2,3) && !pm.contains(3,2)
		&& !pm.contains(2,5) && !pm.contains(5,2)
		&& !pm.contains(2,7) && !pm.contains(7,2)
		&& !pm.contains(3,4) && !pm.contains(4,3)
		&& !pm.contains(3,5) && !pm.contains(5,3)
		&& !pm.contains(3,6) && !pm.contains(6,3)
		&& !pm.contains(3,7) && !pm.contains(7,3)
		&& !pm.contains(3,8) && !pm.contains(8,3)
		&& !pm.contains(4,5) && !pm.contains(5,4)
		&& !pm.contains(4,7) && !pm.contains(7,4)
		&& !pm.contains(5,6) && !pm.contains(6,5)
		&& !pm.contains(5,7) && !pm.contains(7,5)
		&& !pm.contains(5,8) && !pm.contains(8,5)
		&& !pm.contains(6,7) && !pm.contains(7,6)
		&& !pm.contains(7,8) && !pm.contains(8,7)
		&& pm1.contains(0,2) && pm1.contains(0,2)
		&& pm1.contains(0,4) && pm1.contains(0,4)
		&& pm1.contains(0,6) && pm1.contains(0,6)
		&& pm1.contains(0,8) && pm1.contains(0,8)
		&& pm1.contains(2,4) && pm1.contains(2,4)
		&& pm1.contains(2,6) && pm1.contains(2,6)
		&& pm1.contains(2,8) && pm1.contains(2,8)
		&& pm1.contains(4,6) && pm1.contains(4,6)
		&& pm1.contains(4,8) && pm1.contains(4,8)
		&& pm1.contains(6,8) && pm1.contains(6,8)
		&& (pm1(0,2) == 13) && (pm1(0,2) == 13)
		&& (pm1(0,4) == 15) && (pm1(0,4) == 15)
		&& (pm1(0,6) == 17) && (pm1(0,6) == 17)
		&& (pm1(0,8) == 19) && (pm1(0,8) == 19)
		&& (pm1(2,4) == 35) && (pm1(2,4) == 35)
		&& (pm1(2,6) == 37) && (pm1(2,6) == 37)
		&& (pm1(2,8) == 39) && (pm1(2,8) == 39)
		&& (pm1(4,6) == 57) && (pm1(4,6) == 57)
		&& (pm1(4,8) == 59) && (pm1(4,8) == 59)
		&& (pm1(6,8) == 79) && (pm1(6,8) == 79)
	);
}



}  // namespace test
}  // namespace am


#endif
