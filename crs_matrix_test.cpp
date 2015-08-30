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

#include "crs_matrix.h"

#include <vector>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <random>


namespace am {
namespace test {


namespace crs_matrix_test {

//-------------------------------------------------------------------
template<class T, class NA = static_value<T,-1>>
class fixture_type
{
public:
    using value_type   = T;
    using matrix_type  = crs_matrix<value_type,NA>;
    using index_type   = typename matrix_type::size_type;

    struct triple_type {
        triple_type(index_type r, index_type c, value_type v):
            row(r), col(c), val(std::move(v))
        {}

        index_type row;
        index_type col;
        value_type val;
    };

    struct sorter {
        bool operator() (const triple_type& a, const triple_type& b) {
            if(a.row < b.row) return true;
            if(a.row > b.row) return false;
            return a.col < b.col;
        };
    };

    using value_vector = std::vector<triple_type>;
    using value_set    = std::set<triple_type,sorter>;

    fixture_type(std::initializer_list<triple_type> il):
        vals_(il), set_{}
    { make_set(); }

    fixture_type(const std::vector<triple_type>& v):
        vals_(v), set_{}
    { make_set(); }

    fixture_type(std::vector<triple_type>&& v):
        vals_(std::move(v)), set_{}
    { make_set(); }

    const value_vector& original_values() const noexcept { return vals_; }
    const value_set&    expected_values() const noexcept { return set_; }

private:
    void make_set() {
        for(auto i = vals_.rbegin(); i != vals_.rend(); ++i)
            set_.insert(*i);
    }

    value_vector vals_;
    value_set    set_;
};



//-------------------------------------------------------------------
template<class T, class NA>
void check_raw_values_column_indices(const fixture_type<T,NA>& fix,
                                     const crs_matrix<T,NA>& m)
{
    if(fix.expected_values().empty()) return;

    if(m.size() != fix.expected_values().size()) {
        throw std::logic_error{"crs_matrix, value storage: wrong size"};
    }

    int i = 0;
    for(const auto& x : fix.expected_values()) {
        if(m[i] != x.val)
            throw std::logic_error{"crs_matrix, value storage"};

        if(m.begin_col_indices()[i] != x.col)
            throw std::logic_error{"crs_matrix, column indices"};
        ++i;
    }
}



//-------------------------------------------------------------------
template<class T, class NA>
void check_indexed_access(const fixture_type<T,NA>& fix,
                          const crs_matrix<T,NA>& m)
{
    using idx_t    = typename fixture_type<T,NA>::index_type;

    if(fix.expected_values().empty()) return;

    //check indexed access of stored elements
    for(const auto& x : fix.expected_values()) {
        if(m(x.row, x.col) != x.val)
            throw std::logic_error{"crs_matrix, indexed access of stored values"};
    }

    //check indexed access of non-stored elements
    for(idx_t r = 0; r < m.rows(); ++r) {
        for(idx_t c = 0, cols = m.cols(); c < cols; ++c) {
            bool stored = false;

            for(const auto& x : fix.expected_values()) {
                if(x.row == r && x.col == c) {
                    stored = true;
                    break;
                }
            }

            if( !stored &&
                (m(r,c) != m.na_value() || m.has(r,c)) )
            {
                throw std::logic_error{"crs_matrix, indexed access of n/a values"};
            }
        }
    }
}



//-------------------------------------------------------------------
template<class T, class NA>
void check_find_and_index_queries(const fixture_type<T,NA>& fix,
                                  const crs_matrix<T,NA>& m)
{
    using idx_t = typename crs_matrix<T,NA>::size_type;

    if(fix.expected_values().empty()) return;

    //check indexed access of stored elements
    for(const auto& x : fix.expected_values()) {
        auto it = m.find(x.row, x.col);
        if(it == m.end() || *it != x.val)
            throw std::logic_error{"crs_matrix, find(row,col) of stored values"};

        if(m.col_index(it) != x.col) {
            std::cout << x.row << ", " << x.col << " != " << m.col_index(it) << std::endl;
            throw std::logic_error{"crs_matrix, col_index(iterator)"};
        }

        if(m.row_index(it) != x.row)
            throw std::logic_error{"crs_matrix, row_index(iterator)"};

        auto idx = m.index(it);
        if(idx.first != x.row || idx.second != x.col)
            throw std::logic_error{"crs_matrix, index(iterator)"};
    }

    //check indexed access of non-stored elements
    for(idx_t r = 0; r < m.rows(); ++r) {
        for(idx_t c = 0, cols = m.cols(); c < cols; ++c) {
            bool stored = false;
            for(const auto& x : fix.expected_values()) {
                if(x.row == r && x.col == c) {
                    stored = true;
                    break;
                }
            }
            if(!stored && (m.find(r,c) != m.end()))
                throw std::logic_error{"crs_matrix, find of n/a values"};
        }
    }
}



//-------------------------------------------------------------------
template<class T, class NA>
void run_tests(const fixture_type<T,NA>& fix)
{
    using mat_t = typename fixture_type<T,NA>::matrix_type;
    auto m = mat_t{};

    for(const auto& x : fix.original_values()) {
        m.insert(x.row, x.col, x.val);
    }

//    std::cout << "--------------------------\n" << m << std::endl;

    if(fix.expected_values().empty() != m.empty())
        throw std::logic_error{"crs_matrix, emptiness"};

    check_raw_values_column_indices(fix, m);
    check_indexed_access(fix, m);
    check_find_and_index_queries(fix, m);

}



} // namespace crs_matrix_test



//-------------------------------------------------------------------
void crs_matrix_correctness()
{
    using namespace crs_matrix_test;

    using val_t = int;
    using fix_t = fixture_type<val_t>;
    using idx_t = fix_t::index_type;
    using t     = fix_t::triple_type;


    //only (0,0)
    run_tests(fix_t{ t{0,0, 11} });

    //one element at (r > 0, c > 0)
    run_tests(fix_t{ t{5,3, 53} });

    //inserting elements in sorted order (one per row)
    run_tests(fix_t{
        t{0,2,  2}, t{1,6, 16}, t{2,4, 24}, t{3,1, 31},
        t{4,2, 42}, t{5,3, 53}, t{6,7, 67}, t{7,0, 70}, t{8,1, 81} });

    //inserting elements in unsorted order (one per row)
    run_tests(fix_t{
        t{5,3, 53}, t{1,6, 16}, t{8,1, 81}, t{0,2,  2},
        t{6,7, 67}, t{7,0, 70}, t{4,2, 42}, t{3,1, 31}, t{2,4, 24} });

    //insert diagonal elements only (in order)
    {
        auto v = std::vector<t>{};
        v.reserve(30);
        for(int i = 0; i < 30; ++i)
            v.emplace_back(i,i, i);

        run_tests(fix_t{std::move(v)});

    }
    //insert diagonal elements only (backwards)
    {
        auto v = std::vector<t>{};
        v.reserve(30);
        for(int i = 29; i >= 0; --i)
            v.emplace_back(i,i, i);

        run_tests(fix_t{std::move(v)});
    }

    //inserting elements in unsorted order (2 per row, newest last in row)
    run_tests(fix_t{
        t{5,3, 53}, t{2,0, 20}, t{3,2, 32}, t{1,6, 16}, t{4,0, 40}, t{8,1, 81},
        t{0,2,  2}, t{5,4, 54}, t{2,1, 21}, t{3,8, 38}, t{1,8, 18}, t{4,8, 48},
        t{8,3, 83}, t{0,1, 1}
    });

    //inserting elements in unsorted order (2 per row, newest first in row)
    run_tests(fix_t{
        t{5,4, 54}, t{2,1, 21}, t{3,8, 38}, t{1,8, 18}, t{4,8, 48}, t{8,3, 83},
        t{0,1,  1}, t{5,3, 53}, t{2,0, 20}, t{3,2, 32}, t{1,6, 16}, t{4,0, 40},
        t{8,1, 81}, t{0,2, 2}
    });

    //inserting elements in unsorted order (3 per row: 1st, 3rd, 2nd)
    run_tests(fix_t{
        t{6,6, 66}, t{2,0, 20}, t{3,2, 32}, t{1,4, 14}, t{4,0, 40}, t{8,1, 81},
        t{0,2,  2}, t{6,8, 68}, t{2,3, 23}, t{3,8, 38}, t{1,8, 18}, t{4,8, 48},
        t{8,3, 83}, t{0,3,  3}, t{6,7, 67}, t{2,1, 21}, t{3,5, 35}, t{1,6, 16},
        t{4,4, 44}, t{8,2, 82}, t{0,1, 1}
    });

    //inserting elements in unsorted order (4 per row: 1st, 4th, 3rd, 2nd)
    run_tests(fix_t{
        t{6,5, 65}, t{2,0, 20}, t{3,2, 32}, t{1,4, 14}, t{4,0, 40}, t{8,1, 81},
        t{0,1,  1}, t{6,8, 68}, t{2,3, 23}, t{3,8, 38}, t{1,8, 18}, t{4,8, 48},
        t{8,4, 84}, t{0,5,  5}, t{6,7, 67}, t{2,2, 22}, t{3,6, 36}, t{1,7, 17},
        t{4,5, 45}, t{8,3, 83}, t{0,4,  4}, t{6,6, 66}, t{2,1, 21}, t{3,5, 35},
        t{1,6, 16}, t{4,4, 44}, t{8,2, 82}, t{0,2,  2}
    });

    //inserting elements in unsorted order (many per row)
    run_tests(fix_t{
        t{5,3, 53}, t{1,6, 16}, t{8,1, 81}, t{0,2,  2}, t{5,4, 54}, t{1,8, 18},
        t{8,3, 83}, t{0,1,  1}, t{6,4, 64}, t{1,1, 11}, t{8,6, 86}, t{0,6,  6},
        t{6,1, 61}, t{3,3, 33}, t{4,1, 41}, t{4,2, 42}
    });

    //inserting elements / modify
    run_tests(fix_t{
        t{0,2, 96}, t{1,6, 97}, t{2,4, 98}, t{3,1, 99},
        t{0,2,  2}, t{1,6, 16}, t{2,4, 24}, t{3,1, 31}
    });

    //full matrix
    {
        auto v = std::vector<t>{};
        constexpr int n = 30;
        v.reserve(n);
        for(int i = 0; i < n; ++i)
            for(int j = 0; j < n; ++j)
                v.emplace_back(i,j, 10*i+j);

        run_tests(fix_t{std::move(v)});
    }

    //inserting many random elements in unsorted order
    {
        auto urng = std::mt19937{};
        auto idxDistr = std::uniform_int_distribution<idx_t>{0,100};
        auto valDistr = std::uniform_int_distribution<val_t>{0,100};

        auto v = std::vector<t>{};
        constexpr int n = 1000;
        v.reserve(n);
        for(int i = 0; i < n; ++i) {
            v.emplace_back(idxDistr(urng), idxDistr(urng), valDistr(urng));
        }
        run_tests(fix_t{std::move(v)});
    }


//    if(sum != -190)
//        throw std::logic_error{"crs_matrix, pure diagonal, indexed access"};
}

} // namespace test
} // namespace an

#endif


