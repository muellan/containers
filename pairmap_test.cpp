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

#include <map>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <stdexcept>

#include "pairmap.h"


namespace am {
namespace test {


//-------------------------------------------------------------------
void pairmap_subranges_correctness()
{
    //expected range results
    std::map<std::pair<int,int>,int> rr;
    rr[{0,0}] = 144;  rr[{0,1}] = 356;  rr[{0,2}] = 615;  rr[{0,3}] = 900;
    rr[{0,4}] = 1190; rr[{0,5}] = 1464; rr[{0,6}] = 1701; rr[{0,7}] = 1880;
    rr[{0,8}] = 1980; rr[{0,9}] = 1980; rr[{1,1}] = 224;  rr[{1,2}] = 496;
    rr[{1,3}] = 795;  rr[{1,4}] = 1100; rr[{1,5}] = 1390; rr[{1,6}] = 1644;
    rr[{1,7}] = 1841; rr[{1,8}] = 1960; rr[{1,9}] = 1980;
    rr[{2,2}] = 295;  rr[{2,3}] = 618;  rr[{2,4}] = 948;  rr[{2,5}] = 1264;
    rr[{2,6}] = 1545; rr[{2,7}] = 1770; rr[{2,8}] = 1918; rr[{2,9}] = 1968;
    rr[{3,3}] = 357;  rr[{3,4}] = 722;  rr[{3,5}] = 1074; rr[{3,6}] = 1392;
    rr[{3,7}] = 1655; rr[{3,8}] = 1842; rr[{3,9}] = 1932;
    rr[{4,4}] = 410;  rr[{4,5}] = 808;  rr[{4,6}] = 1173; rr[{4,7}] = 1484;
    rr[{4,8}] = 1720; rr[{4,9}] = 1860;
    rr[{5,5}] = 454;  rr[{5,6}] = 876;  rr[{5,7}] = 1245; rr[{5,8}] = 1540;
    rr[{5,9}] = 1740;
    rr[{6,6}] = 489;  rr[{6,7}] = 926;  rr[{6,8}] = 1290; rr[{6,9}] = 1560;
    rr[{7,7}] = 515;  rr[{7,8}] = 958;  rr[{7,9}] = 1308;
    rr[{8,8}] = 532;  rr[{8,9}] = 972;
    rr[{9,9}] = 540;


    pairmap<int,9> pm;
    pm = 0;

    for(size_t i = 0; i <= pm.max_index(); ++i) {
        for(size_t j = i+1; j <= pm.max_index(); ++j) {
            pm(i,j) = 10*(i+1) + j+1;
        }
    }

    for(size_t f = 0; f <= pm.max_index(); ++f) {
        for(size_t l = f; l <= pm.max_index(); ++l) {
            auto r = pm.subrange(f,l);

//            std::cout << "[" << f << "," << l << "] : "
//                << std::accumulate(r.begin(), r.end(), 0)
//                << " == " << rr[{f,l}]
//                << std::endl;

            if(std::accumulate(r.begin(), r.end(), 0) != rr[{f,l}]) {
                throw std::logic_error("am::pairmap subrange iteration");
            }
        }
    }
}



//-------------------------------------------------------------------
void pairmap_correctness()
{
    pairmap_subranges_correctness();


    pairmap<int,8> pm;
    pm = 0;

    for(size_t i = 0; i <= pm.max_index(); ++i) {
        for(size_t j = i+1; j <= pm.max_index(); ++j) {
            pm(i,j) = 10*(i+1) + j+1;
        }
    }

    pairmap<int,7> pm3;
    for(auto& x : pm3) {x = 11; }
    auto pm3sum = std::accumulate(begin(pm3), end(pm3), 0);

    auto pm2 = std::move(pm);
    auto pm1 = pairmap<int,8>{pm2};

    if( !( (std::accumulate(pm1.begin(0), pm1.end(0), 0) == 124)
        && (std::accumulate(pm1.begin(1), pm1.end(1), 0) == 194)
        && (std::accumulate(pm1.begin(2), pm1.end(2), 0) == 255)
        && (std::accumulate(pm1.begin(3), pm1.end(3), 0) == 307)
        && (std::accumulate(pm1.begin(4), pm1.end(4), 0) == 350)
        && (std::accumulate(pm1.begin(5), pm1.end(5), 0) == 384)
        && (std::accumulate(pm1.begin(6), pm1.end(6), 0) == 409)
        && (std::accumulate(pm1.begin(7), pm1.end(7), 0) == 425)
        && (std::accumulate(pm1.begin(8), pm1.end(8), 0) == 432) ))
    {
        throw std::logic_error("am::pairmap iteration");
    }

    if( !( (pm3sum == int(11 * pm3.size()))
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
        && (pm1(7,8) == 89) && (pm1(7,8) == 89) ))
    {
        throw std::logic_error("am::pairmap element access");
    }
}



}  // namespace test
}  // namespace am


#endif
