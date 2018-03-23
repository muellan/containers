/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include "triangle_matrix.h"

#include <vector>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <random>

using namespace am;


//-------------------------------------------------------------------
struct value_t {
    value_t(int y = -1) : x_{y} { ++n_; }
    value_t(std::size_t y) : x_{static_cast<int>(y)} { ++ n_; };
    value_t(const value_t& a) : x_ {a.x_} { ++n_; }
    value_t(value_t&& a) : x_ {a.x_} { ++n_; }

    value_t& operator = (const value_t&) = default;
    value_t& operator = (value_t&&) = default;

    ~value_t() { --n_; }

    static int instances() {return n_; }
    static void reset_instance_counter() { n_ = 0; }

    operator int() const noexcept {return x_; };

private:
    int x_ = -1;
    static int n_;

    inline friend std::ostream&
    operator << (std::ostream& os, const value_t& a) {
        return (os << a.x_);
    }
};

int value_t::n_ = 0;



//-------------------------------------------------------------------
triangle_matrix<value_t>
make_testing_matrix(triangle_matrix<value_t>::size_type n)
{
    using size_type = triangle_matrix<value_t>::size_type;

    value_t::reset_instance_counter();

    triangle_matrix<value_t> m;
    if(n > 0) {
        m.rows(n);
        for(size_type i = 1; i < n+1; ++i) {
            for(size_type j = 0; j < i; ++j) {
                m(i,j) = value_t(10*i + j);
            }
        }
        //verify content values
        for(size_type r = 1; r < n+1; ++r) {
            for(size_type c = 0; c < r; ++c) {
                if(m(r,c) != value_t(10*r + c))
                    throw std::logic_error{"triangle_matrix: assignment"};
            }
        }
    }
    else {
        if(!m.empty())
            throw std::logic_error{"triangle_matrix::empty"};
    }

    //verify content element construction
    if(value_t::instances() != int(m.size())) {
        throw std::logic_error{"triangle_matrix: construction memory management"};
    }

    return m;
}



//-------------------------------------------------------------------
void triangle_matrix_resizing_correctness()
{
    using size_type = decltype(make_testing_matrix(1).size());

    constexpr size_type n = 20;

    const auto insertedValue = value_t(99);

    for(size_type i = 0; i <= n; ++i) {
        for(size_type q = 0; q <= 2*n; ++q) {

            auto m = make_testing_matrix(i);

//            std::cout << "-------------------------------------------\n" << m
//                      << "resize " << i << " -> " << q << std::endl;

            m.rows(q, insertedValue);

//            std::cout << m << std::endl;

            //old content
            for(size_type r = 1; r <= std::min(i,m.rows()); ++r) {
                for(size_type c = 0; c < r; ++c) {
                    if(m(r,c) != value_t(10*r + c))
                        throw std::logic_error{"triangle_matrix: resizing, old values"};
                }
            }
            //new rows
            if(m.rows() > i) {
                for(size_type r = i+1; r <= m.rows(); ++r) {
                    for(size_type c = 0; c < r; ++c) {
                        if(m(r,c) != insertedValue)
                            throw std::logic_error{"triangle_matrix: resizing, new values"};
                    }
                    //verify content element construction
                    if(value_t::instances() != int(m.size())) {
                        throw std::logic_error{"triangle_matrix: resizing memory management"};
                    }
                }
            }
        }
    }
}



//-------------------------------------------------------------------
void triangle_matrix_row_iterator_correctness(
    const triangle_matrix<value_t>& m)
{
    using size_type = decltype(make_testing_matrix(1).size());

    for(size_type r = 1; r <= m.rows(); ++r) {
        size_type c = 0;
        for(auto i = m.begin_row(r), e = m.end_row(r); i != e; ++i, ++c) {
            auto v = *i;
            if(c >= m.cols() || v != value_t(10*r + c)) {
                //                    std::cerr << '\n' << m << "\n@(" << r << "," << c << ") = "
                //                              << v << " != " << value_t(10*r + c) << std::endl;
                throw std::logic_error {"triangle_matrix: row iterator"};
            }
        }

        c = 0;
        for(const auto& v : m.row(r)) {
            if(c >= m.cols() || v != value_t(10*r + c)) {
                throw std::logic_error {"triangle_matrix: row range"};
            }
            ++c;
        }
    }
}



//-------------------------------------------------------------------
void triangle_matrix_col_iterator_correctness(
    const triangle_matrix<value_t>& m)
{
    using size_type = decltype(make_testing_matrix(1).size());

    for(size_type c = 0; c < m.cols(); ++c) {
        size_type r = c + 1;
        for(auto i = m.begin_col(c), e = m.end_col(c); i != e; ++i, ++r) {
            auto v = *i;
            if(r > m.rows() || v != value_t(10*r + c)) {
                throw std::logic_error {"triangle_matrix: col iterator"};
            }
        }

        r = c + 1;
        for(const auto& v : m.col(c)) {
            if(r > m.rows() || v != value_t(10*r + c)) {
                throw std::logic_error {"triangle_matrix: col range"};
            }
            ++r;
        }
    }
}



//-------------------------------------------------------------------
void triangle_matrix_index_iterator_correctness(
    const triangle_matrix<value_t>& m)
{
    using size_type = decltype(make_testing_matrix(1).size());

    for(size_type k = 0; k <= m.rows(); ++k) {
        size_type r = k < 1 ? 1 : k;
        size_type c = 0;

        bool l = false;
        for(auto i = m.begin_at(k), e = m.end_at(k); i != e; ++i) {
            auto v = *i;

            if(r > m.rows() || c >= m.cols() || v != value_t(10*r + c)) {
                throw std::logic_error {"triangle_matrix: index iterator"};
            }
            if(k < 1) {
                ++r;
            } else {
                if(!l && c < r-1) {
                    ++c;
                } else if(!l && c == r-1) {
                    ++c;
                    ++r;
                    l = true;
                } else {
                    ++r;
                }
            }
        }

        r = k < 1 ? 1 : k;
        c = 0;
        l = false;
        for(const auto& v : m.index_interval(k)) {
            if(r > m.rows() || c >= m.cols() || v != value_t(10*r + c)) {
                throw std::logic_error {"triangle_matrix: index index_range"};
            }
            if(k < 1) {
                ++r;
            } else {
                if(!l && c < r-1) {
                    ++c;
                } else if(!l && c == r-1) {
                    ++c;
                    ++r;
                    l = true;
                } else {
                    ++r;
                }
            }
        }
    }
}



//-------------------------------------------------------------------
void triangle_matrix_index_range_correctness(
    const triangle_matrix<value_t>& m)
{
    using size_type = decltype(make_testing_matrix(1).size());

    for(size_type i = 0; i <= m.rows(); ++i) {
        for(size_type j = i; j <= m.rows(); ++j) {
//            std::cout << "[" << i << "," << j << "]" << std::endl;

            size_type r = i < 1 ? 1 : i;
            size_type c = 0;

            for(const auto& v : m.index_interval(i,j)) {
//                std::cout << "  (" << r << "," << c << ") = " << v << std::endl;

                if(r > m.rows() || c >= m.cols() || v != value_t(10*r + c)) {
                    throw std::logic_error {"triangle_matrix: index_range"};
                }

                ++c;
                if((c > j || c >= r)) {
                    if(r < j)
                        c = 0;
                    else
                        c = i;
                    ++r;
                }
            }
        }
    }
}



//-------------------------------------------------------------------
void triangle_matrix_index_query_correctness(
    const triangle_matrix<value_t>& m)
{
    using size_type = decltype(make_testing_matrix(1).size());


//    std::cout << "----------------------------------------\n" << m << std::endl;

    auto it = begin(m);
    for(size_type r = 1; r <= m.rows(); ++r) {
        for(size_type c = 0; c < r; ++c) {

            auto idx = m.index_of(it);

            if(idx.first != r || idx.second != c) {
//                std::cerr << "(" << r << "," << c << ") != ("
//                          << idx.first << "," << idx.second << ")" << std::endl;

                throw std::logic_error {"triangle_matrix::index_of(const_iterator)"};
            }
            ++it;
        }
    }

}



//-------------------------------------------------------------------
void triangle_matrix_construction_and_iterators_correctness()
{
    using size_type = decltype(make_testing_matrix(1).size());

    for(size_type n = 1; n < 100; ++n) {
//        std::cout << "n = " << n << std::endl;

        auto m = make_testing_matrix(n-1);
//        std::cout << '\n' << m << std::endl;

        triangle_matrix_row_iterator_correctness(m);

        triangle_matrix_col_iterator_correctness(m);

        triangle_matrix_index_iterator_correctness(m);

        triangle_matrix_index_range_correctness(m);

        triangle_matrix_index_query_correctness(m);
    }


}




//-------------------------------------------------------------------
void triangle_matrix_insertion_correctness(bool reserveBefore = false)
{
    using size_type = decltype(make_testing_matrix(1).size());

    constexpr size_type n = 10;

    for(size_type i = 0; i <= n; ++i) {
        for(size_type q = 1; q <= 2*n; ++q) {

            auto m = make_testing_matrix(n-1);

//            std::cout << "-------------------------------------------\n" << m
//                      << "insert (" << i << "," << q << ")" << std::endl;

            if(reserveBefore) {
                m.reserve_rows(m.rows() + q);
            }
            const auto insertedValue = value_t(99);
            m.insert_at(i, q, insertedValue);

//            std::cout << m << std::endl;

            //verify result
            const auto rows = m.rows();
            for(size_type r = 1; r <= rows; ++r) {
                for(size_type c = 0; c < r; ++c) {
                    //above inserted rows
                    if(r < i) {
                        if(m(r, c) != value_t(10 * r + c))
                            throw std::logic_error {
                                "triangle_matrix::insert_at: wrong content above inserted rows"};
                    }
                    else {
                        //inserted rows
                        if(r >= i && r < i + q) {
                            if(m(r, c) != insertedValue) {
//                                std::cerr << '\n' << m << "\n@(" << r << "," << c << ")" << std::endl;
                                throw std::logic_error {
                                    "triangle_matrix::insert_at: wrong content in inserted rows"};
                            }
                        }
                        //below inserted rows
                        else {
                            //before inserted columns
                            if(c < i) {
                                if(m(r, c) != value_t(10 * (r - q) + c)) {
//                                    std::cerr << '\n' << m << "\n@(" << r << "," << c << ")" << std::endl;
                                    throw std::logic_error {
                                        "triangle_matrix::insert_at: wrong content before inserted columns"};
                                }
                            }
                            else if(c >= i && c < i + q) {
                                if(m(r, c) != insertedValue) {
//                                    std::cerr << '\n' << m << "\n@(" << r << "," << c << ")" << std::endl;
                                    throw std::logic_error {
                                        "triangle_matrix::insert_at: wrong content in inserted columns"};
                                }
                            }
                            //after inserted columns
                            else {
                                if(m(r, c) != value_t(10 * (r - q) + (c - q))) {
//                                    std::cerr << '\n' << m << "\n@(" << r << "," << c << ")" << std::endl;
                                    throw std::logic_error {
                                        "triangle_matrix::insert_at: wrong content after inserted columns"};
                                }
                            }
                        }
                    }
                }
            }

//            std::cerr << "\nvalue_t::instances() = " << (value_t::instances()-1)
//                      << " =?= m.size() = " << m.size()
//                      << "\ni = " << i << ", q = " << q << std::endl;

            if((value_t::instances() - 1) != int(m.size())) {
                //insertedValue ------^
                throw std::logic_error {
                    "triangle_matrix::insert_at: memory management"};
            }
        }
    }
}



//-------------------------------------------------------------------
void triangle_matrix_erasure_correctness()
{
    using size_type = decltype(make_testing_matrix(1).size());

    constexpr size_type n = 10;

    for(size_type i = 0; i <= n; ++i) {
        for(size_type j = i; j <= n; ++j) {

            auto m = make_testing_matrix(n-1);

//            std::cout << "-------------------------------------------\n"
//                      << "erase (" << i << "," << q << ")\n";

            m.erase_at(i, j);

            const size_type q = j - i + 1;

            //verify result
            const auto rows = m.rows();
            for(size_type r = 1; r <= rows; ++r) {
                for(size_type c = 0; c < r; ++c) {
                    //above deleted rows
                    if(r < i) {
                        if(m(r, c) != value_t(10 * r + c))
                            throw std::logic_error {
                                "triangle_matrix::erase_at: wrong content above deleted rows"};
                    }
                    //below deleted rows
                    else {
                        //before deleted columns
                        if(c < i) {
                            if(m(r, c) != value_t(10 * (r + q) + c)) {
//                              std::cerr << '\n' << m << "\n@(" << r << "," << c << ")" << std::endl;
                                throw std::logic_error {
                                    "triangle_matrix::erase_at: wrong content before deleted columns"};
                            }
                        }
                        //after deleted columns
                        else {
                            if(m(r, c) != value_t(10 * (r + q) + (c + q))) {
//                              std::cerr << '\n' << m << "\n@(" << r << "," << c << ")" << std::endl;
                                throw std::logic_error {
                                    "triangle_matrix::erase_at: wrong content after deleted columns"};
                            }
                        }
                    }
                }
            }

            if(value_t::instances() != int(m.size())) {
                throw std::logic_error {
                    "triangle_matrix::erase_at: memory management"};
            }
        }
    }
}



//-------------------------------------------------------------------
int main()
{
    try {
        triangle_matrix_construction_and_iterators_correctness();
        triangle_matrix_resizing_correctness();
        triangle_matrix_insertion_correctness(false);
        triangle_matrix_insertion_correctness(true);
        triangle_matrix_erasure_correctness();
    }
    catch(std::exception& e) {
        std::cerr << e.what();
        return 1;
    }
}
