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

#include "dynpairmap.h"


namespace am {
namespace test {


//-------------------------------------------------------------------
bool dynpairmap_correct()
{
	dynpairmap<int> pm;

	pm.resize_indices(9);
	pm = 0;

	for(size_t i = 0; i < pm.index_count(); ++i) {
		for(size_t j = i+1; j < pm.index_count(); ++j) {
			pm(i,j) = 10*(i+1) + j+1;
		}
	}

	dynpairmap<int> pm3;
	pm3.resize_indices(7);
	for(auto& x : pm3) {x = 11; }
	auto pm3sum = std::accumulate(begin(pm3), end(pm3), 0);

	auto pm2 = std::move(pm);
	auto pm1 = dynpairmap<int>{pm2};

//	std::cout << pm2 << std::endl;
//	pm2.swap(1,2);
//	std::cout << pm2 << std::endl;

	return (true
		&& (std::accumulate(pm1.begin(0), pm1.end(0), 0) == 124)
		&& (std::accumulate(pm1.begin(1), pm1.end(1), 0) == 194)
		&& (std::accumulate(pm1.begin(2), pm1.end(2), 0) == 255)
		&& (std::accumulate(pm1.begin(3), pm1.end(3), 0) == 307)
		&& (std::accumulate(pm1.begin(4), pm1.end(4), 0) == 350)
		&& (std::accumulate(pm1.begin(5), pm1.end(5), 0) == 384)
		&& (std::accumulate(pm1.begin(6), pm1.end(6), 0) == 409)
		&& (std::accumulate(pm1.begin(7), pm1.end(7), 0) == 425)
		&& (std::accumulate(pm1.begin(8), pm1.end(8), 0) == 432)
		&& (pm3sum == int(11 * (pm3.index_count() * (pm3.index_count() - 1)) / 2))
		&& (pm1(0,1) == 12) && (pm1(0,1) == 12)
		&& (pm1(0,2) == 13) && (pm1(0,2) == 13)
		&& (pm1(0,3) == 14) && (pm1(0,3) == 14)
		&& (pm1(0,4) == 15) && (pm1(0,4) == 15)
		&& (pm1(0,5) == 16) && (pm1(0,5) == 16)
		&& (pm1(0,6) == 17) && (pm1(0,6) == 17)
		&& (pm1(0,7) == 18) && (pm1(0,7) == 18)
		&& (pm1(0,8) == 19) && (pm1(0,8) == 19)
		&& (pm1(1,2) == 23) && (pm1(1,2) == 23)
		&& (pm1(1,3) == 24) && (pm1(1,3) == 24)
		&& (pm1(1,4) == 25) && (pm1(1,4) == 25)
		&& (pm1(1,5) == 26) && (pm1(1,5) == 26)
		&& (pm1(1,6) == 27) && (pm1(1,6) == 27)
		&& (pm1(1,7) == 28) && (pm1(1,7) == 28)
		&& (pm1(1,8) == 29) && (pm1(1,8) == 29)
		&& (pm1(2,3) == 34) && (pm1(2,3) == 34)
		&& (pm1(2,4) == 35) && (pm1(2,4) == 35)
		&& (pm1(2,5) == 36) && (pm1(2,5) == 36)
		&& (pm1(2,6) == 37) && (pm1(2,6) == 37)
		&& (pm1(2,7) == 38) && (pm1(2,7) == 38)
		&& (pm1(2,8) == 39) && (pm1(2,8) == 39)
		&& (pm1(3,4) == 45) && (pm1(3,4) == 45)
		&& (pm1(3,5) == 46) && (pm1(3,5) == 46)
		&& (pm1(3,6) == 47) && (pm1(3,6) == 47)
		&& (pm1(3,7) == 48) && (pm1(3,7) == 48)
		&& (pm1(3,8) == 49) && (pm1(3,8) == 49)
		&& (pm1(4,5) == 56) && (pm1(4,5) == 56)
		&& (pm1(4,6) == 57) && (pm1(4,6) == 57)
		&& (pm1(4,7) == 58) && (pm1(4,7) == 58)
		&& (pm1(4,8) == 59) && (pm1(4,8) == 59)
		&& (pm1(5,6) == 67) && (pm1(5,6) == 67)
		&& (pm1(5,7) == 68) && (pm1(5,7) == 68)
		&& (pm1(5,8) == 69) && (pm1(5,8) == 69)
		&& (pm1(6,7) == 78) && (pm1(6,7) == 78)
		&& (pm1(6,8) == 79) && (pm1(6,8) == 79)
		&& (pm1(7,8) == 89) && (pm1(7,8) == 89)
	);
}



}  // namespace test
}  // namespace am


#endif
