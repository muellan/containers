/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2016 André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include "compressed_multiset.h"

#include <stdexcept>
#include <iostream>

namespace test {

using namespace am;

//-------------------------------------------------------------------
struct cms_test_item
{
    int x;
};

//---------------------------------------------------------
struct cms_test_compare
{
    bool operator() (const cms_test_item& a, const cms_test_item& b) const {
        return (a.x < b.x);
    }
};



//-------------------------------------------------------------------
bool test_compressed_multiset_ok(const compressed_multiset<cms_test_item,cms_test_compare>& r)
{
    return (
        r.size() == 21 &&
        r.unique() == 6 &&
        r.count(cms_test_item{0}) == 0 &&
        r.count(cms_test_item{1}) == 1 &&
        r.count(cms_test_item{2}) == 2 &&
        r.count(cms_test_item{3}) == 3 &&
        r.count(cms_test_item{4}) == 4 &&
        r.count(cms_test_item{5}) == 5 &&
        r.count(cms_test_item{6}) == 6);
}



//-------------------------------------------------------------------
void compressed_multiset_correctness()
{
    auto repo1 =
        compressed_multiset<cms_test_item,cms_test_compare>{
            cms_test_item{2}, cms_test_item{1},
            cms_test_item{2}, cms_test_item{4}
        };

    repo1.insert(cms_test_item{4});
    repo1.insert(cms_test_item{5},5);
    repo1.insert(cms_test_item{3},2);
    repo1.insert(cms_test_item{4});
    repo1.insert(cms_test_item{7}, 2);
    repo1.insert(cms_test_item{3});
    repo1.insert(cms_test_item{8});
    repo1.insert(cms_test_item{6},6);
    repo1.insert(cms_test_item{4});


    auto repo2 =
        compressed_multiset<cms_test_item,cms_test_compare>{
            {cms_test_item{1}, 1},
            {cms_test_item{2}, 2},
            {cms_test_item{3}, 3},
            {cms_test_item{4}, 4},
            {cms_test_item{5}, 5},
            {cms_test_item{6}, 6}
        };

    repo1.erase(cms_test_item{7}, 1);
    repo1.erase(repo1.find(cms_test_item{8}));
    repo1.erase(cms_test_item{7}, 6);

    if(!test_compressed_multiset_ok(repo1) || !test_compressed_multiset_ok(repo2)) {
        throw std::logic_error("am::compressed_multiset initialization");
    }


}


} //namespace test


#endif
