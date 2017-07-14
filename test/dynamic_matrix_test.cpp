/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#include "dynamic_matrix.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <numeric>

using namespace am;


//-------------------------------------------------------------------
struct value_t {
    value_t(int y = -1) : x_ {y} { ++n_; }
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
void test_initialization()
{
    dynamic_matrix<int> m1 = {
        {11, 12, 13},
        {21, 22, 23},
        {31, 32, 33},
        {41, 42, 43}
    };

    dynamic_matrix<int> m2 = {1,2,3};

    #ifdef AM_USE_EXCEPTIONS
        bool caught = false;

        try {
            dynamic_matrix<int> m3 = {{1,2}, {1,2,3}, {1,2}};
        }
        catch(init_incoherent_row_sizes&) {
            caught = true;
        }
    #else
        bool caught = true;
        if(!(
        caught &&
        (m1(0,0) == 11) && (m1(0,1) == 12) && (m1(0,2) == 13) &&
        (m1(1,0) == 21) && (m1(1,1) == 22) && (m1(1,2) == 23) &&
        (m1(2,0) == 31) && (m1(2,1) == 32) && (m1(2,2) == 33) &&
        (m1(3,0) == 41) && (m1(3,1) == 42) && (m1(3,2) == 43) &&
        (m2(0,0) == 1) && (m2(0,1) == 2) && (m2(0,2) == 3) ))
    {
        throw std::logic_error("am::dynamic_matrix initialization");
    }
    #endif
}



//-------------------------------------------------------------------
void test_memory()
{
    {
        dynamic_matrix<value_t> m;
        m.resize(1,2, {0});
        m.resize(6,6, {1});
        m.resize(2,3, {1});
        m.resize(10,10, {1});
        m.resize(2,2, {1});
        m.resize(4,4, {1});
        m.erase_col(1);
        m.erase_row(1);
        m.reserve(20,20);
        m.resize(100,100, {1});

        if(value_t::instances() != int(m.size())) {
            throw std::logic_error("am::dynamic_matrix memory alloc");
        }

        for(int i = 0; i < 100; ++i) {
            m.erase_col(0);

            if(value_t::instances() != int(m.size())) {
                throw std::logic_error("am::dynamic_matrix memory content destruct");
            }
        }

        m.resize(100,100, {1});
        for(int i = 0; i < 100; ++i) {
            m.erase_row(0);

            if(value_t::instances() != int(m.size())) {
                throw std::logic_error("am::dynamic_matrix memory content destruct");
            }
        }

//        std::cout << m << std::endl;
    }

    {
        dynamic_matrix<value_t> m;

        int n = 1;
        for(int i = 0; i < 50; ++i, ++n) {
            m.insert_rows(i,n, i+1);
            m.insert_cols(i,n, i+1);

            if(value_t::instances() != int(m.size())) {
                throw std::logic_error("am::dynamic_matrix memory alloc");
            }
        }
    }

    {
        dynamic_matrix<value_t> m;
        m.resize(210,210);

        int n = 1;
        for(int i = 0; i < 20; ++i, ++n) {
            m.erase_cols(i, 2*i);
            m.erase_rows(i, 2*i);

            if(value_t::instances() != int(m.size())) {
                throw std::logic_error(
                    "am::dynamic_matrix memory dealloc / content destruct");
            }
        }
    }

//    std::cout << "\n# " << A::n << '\n' << std::endl;
    if(value_t::instances() != 0) {
        throw std::logic_error("am::dynamic_matrix memory dealloc / content destruct");
    }
}



//-------------------------------------------------------------------
void test_move()
{
    int sum1 = 0, sum2 = 0, sum3 = 0;
    {
        dynamic_matrix<int> m;
        m.resize(9,9,0);
        int x = 10;
        std::generate(begin(m), end(m), [&]{++x; return x;});
        for(auto i : m) sum1 += i;

        dynamic_matrix<int> mm1 {std::move(m)};
        for(auto i : mm1) sum2 += i;

        m = std::move(mm1);
        for(auto i : m) sum3 += i;
    }

    if((value_t::instances() != 0) || (sum1 != 4131) || (sum2 != 4131) || (sum3 != 4131)) {
        throw std::logic_error("am::dynamic_matrix move");
    }
}



//-------------------------------------------------------------------
void test_resizing()
{
    dynamic_matrix<int> m;
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

    if(! (
        (m(0,0) == 11) && (m(0,1) == 88) &&
        (m(1,0) == 2) && (m(1,1) == 2) &&
        (m(2,0) == 3) && (m(2,1) == 3) &&
        (m(3,0) == 0) && (m(3,1) == 0) ) )
    {
        throw std::logic_error("am::dynamic_matrix resizing");
    }
}



//-------------------------------------------------------------------
void test_iterators()
{
    dynamic_matrix<int> m;
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

    for(auto x : m.rectangle(0,0, m.rows()-1, m.cols()-1)) {
        sum += x;
    }
    sum *= 100;

    for(std::size_t r = 0; r < m.rows(); ++r) {
        for(std::size_t c = 0; c < m.cols(); ++c) {
            for(auto x : m.rectangle(0,0, r,c)) {
                sum += x;
            }
        }
    }

    for(int r =  m.rows()-1; r >= 0; --r) {
        for(int c = m.cols()-1; c >= 0; --c) {
            for(auto x : m.rectangle(r,c, m.rows()-1, m.cols()-1)) {
                sum += x;
            }
        }
    }

    if(sum != 3217308640) {
        throw std::logic_error("am::dynamic_matrix iteration");
    }
}



//-------------------------------------------------------------------
int main()
{
    try {
        test_initialization();
        test_memory();
        test_move();
        test_resizing();
        test_iterators();
    }
    catch(std::exception& e) {
        std::cerr << e.what();
        return 1;
    }
}

