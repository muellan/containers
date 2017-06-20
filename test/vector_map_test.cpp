/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2016 André Müller
 *
 *****************************************************************************/


#include "../include/vector_map.h"

#include <stdexcept>
#include <iostream>


using namespace am;

//-------------------------------------------------------------------
int main()
{
/*
    auto f = vector_map<double,double>
        { {2.,2.}, {10.,5.}, {3.,6.}, {12.,6.}, {4.,10.}, {15.,8.} };


    for(const auto v : f) {
        std::cout << "(" << v.first << "," << v.second << ")" << std::endl;
    }

    std::cout << "----------------------------------" << std::endl;
    f.assign({{1.,1.}, {1.9,10.}, {2.0,10.}, {2.0, 30.}, {2.2, 20.0}, {3.,3.}, {4.,4.}});

    for(const auto v : f) {
        std::cout << "(" << v.first << "," << v.second << ")" << std::endl;
    }


    const auto er = f.equal_range(2);

    std::cout << "2: " << distance(er.first,er.second)
        << " [" << er.first->second
        << "," <<  er.second->second
        << "]" << std::endl;

    std::cout << "2: " << f.count(2)
        << " [" << f.lower_bound(2)->second
        << "," <<  f.upper_bound(2)->second
        << "]" << std::endl;

    f.erase(2);

    std::cout << "2: " << f.count(2)
        << " [" << f.lower_bound(2)->second
        << "," <<  f.upper_bound(2)->second
        << "]" << std::endl;
*/
}

