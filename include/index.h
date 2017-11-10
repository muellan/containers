/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2012-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_CONTAINERS_INDEX_H_
#define AMLIB_CONTAINERS_INDEX_H_

#include <type_traits>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <iterator>
#include <initializer_list>
#include <tuple>


namespace am {

namespace index_detail {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class IntegerSequence, typename IntegerSequence::value_type right>
struct integer_appender;

template<class Int, Int... init, Int last>
struct integer_appender<std::integer_sequence<Int,init...>,last>
{
    using type = std::integer_sequence<Int,init...,last>;
};

template<class IntegerSequence, typename IntegerSequence::value_type right>
using append_integer =
    typename index_detail::integer_appender<IntegerSequence,right>::type;



/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Int, std::size_t size, Int value>
struct uniform_integer_sequence_maker
{
    using type = append_integer<
        typename uniform_integer_sequence_maker<Int,size-1,value>::type,value>;
};

template<class Int, Int value>
struct uniform_integer_sequence_maker<Int,0,value>
{
    using type = std::integer_sequence<Int>;
};


template<class Int, std::size_t size, Int value>
using make_uniform_integer_sequence =
    typename index_detail::uniform_integer_sequence_maker<Int,size,value>::type;



/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<std::size_t n, std::size_t i = 0>
struct index_iteration
{
    template<class Target, class Source>
    static void
    assign_scalar(Target& t, const Source& s) {
        index_iteration<n-1,i+1>::assign_scalar(t,s);
        t[n-1] = s;
    }

    template<class Target, class S, class... Ss>
    static void
    assign_variadic(Target& t, const S& s, Ss&&... ss) {
        t[i] = s;
        index_iteration<n-1,i+1>::assign_variadic(t, std::forward<Ss>(ss)...);
    }

};

//-------------------------------------------------------------------
template<std::size_t n>
struct index_iteration<0,n>
{
    template<class Target, class Source>
    static void
    assign_scalar(Target&, const Source&) {}

    template<class Target, class...S>
    static void
    assign_variadic(Target&, S&&...) {}
};



/*************************************************************************//***
 *
 * @brief maps AND over a variadic list of boolean traits
 *
 *****************************************************************************/
template<class...>
struct all;

template<>
struct all<> : std::true_type {};

template<class T1>
struct all<T1> : T1 {};

template<class T1, class T2>
struct all<T1,T2> :
    std::conditional<T1::value,T2,T1>::type
{};

template<class T1, class T2, class T3, class... Ts>
struct all<T1,T2,T3,Ts...> :
    std::conditional<T1::value, all<T2,T3,Ts...>,T1>::type
{};


}  // namespace index_detail




/*************************************************************************//***
 *
 *
 * @brief array-like type with integral fundamental values as content;
 *        supports addition, subtraction and lexicographic comparisons
 *
 *****************************************************************************/
template<std::size_t n, class T = std::size_t>
class index
{
    static_assert(
        (n > 0) &&
        std::is_fundamental<T>::value &&
        std::is_integral<T>::value,
        "index<n,T> : n must be > 0 and T must be a fundamental integral type");

public:
    //---------------------------------------------------------------
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = const value_type&;
    //-----------------------------------------------------
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    //-----------------------------------------------------
    using iterator        = value_type*;
    using const_iterator  = const value_type*;
    //-----------------------------------------------------
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;


    //---------------------------------------------------------------
    constexpr
    index() noexcept :
        index{index_detail::make_uniform_integer_sequence<int,n,0>{}}
    {}
    //-----------------------------------------------------
    template<class... V, class = typename
        std::enable_if_t<(n > 0) && (n == sizeof...(V)) &&
            index_detail::all<std::is_fundamental<V>...>::value &&
            index_detail::all<std::is_integral<V>...>::value>>
    constexpr
    index(V... vals) noexcept :
        m_{static_cast<value_type>(vals)...}
    {}

    //---------------------------------------------------------------
    index& operator = (const value_type& v) {
        index_detail::index_iteration<n>::assign_scalar(m_, v);
        return *this;
    }


    //---------------------------------------------------------------
    template<class... Values, class =
        typename std::enable_if_t<
            (n > 0) && (n == sizeof...(Values)) &&
            index_detail::all<std::is_fundamental<Values>...>::value &&
            index_detail::all<std::is_integral<Values>...>::value>>
    index&
    assign(Values&&... values)
    {
        index_detail::index_iteration<n>::assign_variadic(
            m_, std::forward<Values>(values)...);

        return *this;
    }


    //---------------------------------------------------------------
    constexpr size_type
    size() const noexcept {
        return n;
    }


    //---------------------------------------------------------------
    reference
    operator[](size_type i) noexcept {
        return m_[i];
    }
    //---------------------------------------------------------
    constexpr const_reference
    operator[](size_type i) const noexcept {
        return m_[i];
    }


    //---------------------------------------------------------------
    template<class X = int, class = std::enable_if_t<(n > 0 && n < 4),X>>
    reference
    x() noexcept {
        return m_[0];
    }
    template<class X = int, class = std::enable_if_t<(n > 0 && n < 4),X>>
    const_reference
    x() const noexcept {
        return m_[0];
    }
    //-----------------------------------------------------
    template<class X = int, class = std::enable_if_t<(n > 1 && n < 4),X>>
    reference
    y() noexcept {
        return m_[1];
    }
    template<class X = int, class = std::enable_if_t<(n > 1 && n < 4),X>>
    const_reference
    y() const noexcept {
        return m_[1];
    }
    //-----------------------------------------------------
    template<class X = int, class = std::enable_if_t<(n == 3),X>>
    reference
    z() noexcept {
        return m_[2];
    }
    template<class X = int, class = std::enable_if_t<(n == 3),X>>
    const_reference
    z() const noexcept {
        return m_[2];
    }


    //---------------------------------------------------------------
    reference
    front() noexcept {
        return m_[0];
    }
    //-----------------------------------------------------
    constexpr const_reference
    front() const noexcept {
        return m_[0];
    }
    //-----------------------------------------------------
    reference
    back() noexcept {
        return m_[n-1];
    }
    //-----------------------------------------------------
    constexpr const_reference
    back() const noexcept {
        return m_[n-1];
    }


    //---------------------------------------------------------------
    void swap(index& other) noexcept {
        std::swap_ranges(begin(), end(), other.begin());
    }


    //---------------------------------------------------------------
    index&
    operator += (const index& other) {
        for(std::size_t i = 0; i < n; ++i) m_[i] += other.m_[i];
        return *this;
    }

    //---------------------------------------------------------------
    index&
    operator -= (const index& other) {
        for(std::size_t i = 0; i < n; ++i) m_[i] -= other.m_[i];
        return *this;
    }

    //---------------------------------------------------------------
    index&
    operator += (value_type offset) {
        for(auto i = begin(); i != end(); ++i) *i += offset;
        return *this;
    }

    //---------------------------------------------------------------
    index&
    operator -= (value_type offset) {
        for(auto i = begin(); i != end(); ++i) *i -= offset;
        return *this;
    }


    //---------------------------------------------------------------
    iterator begin() noexcept {
        return iterator{std::addressof(m_[0])};
    }
    const_iterator begin() const noexcept {
        return const_iterator{std::addressof(m_[0])};
    }
    const_iterator cbegin() const noexcept {
        return const_iterator{std::addressof(m_[0])};
    }
    //-----------------------------------------------------
    inline friend iterator begin(index& i) noexcept {
        return i.begin();
    }
    inline friend const_iterator begin(const index& i) noexcept {
        return i.begin();
    }
    inline friend const_iterator cbegin(const index& i) noexcept {
        return i.cbegin();
    }

    //-----------------------------------------------------
    iterator end() noexcept {
        return iterator(std::addressof(m_[n]));
    }
    const_iterator end() const noexcept    {
        return const_iterator(std::addressof(m_[n]));
    }
    const_iterator cend() const noexcept {
        return const_iterator(std::addressof(m_[n]));
    }
    //-----------------------------------------------------
    inline friend iterator end(index& i) noexcept {
        return i.end();
    }
    inline friend const_iterator end(const index& i) noexcept {
        return i.end();
    }
    inline friend const_iterator cend(const index& i) noexcept {
        return i.cend();
    }

    //-----------------------------------------------------
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    //-----------------------------------------------------
    inline friend reverse_iterator rbegin(index& i) noexcept {
        return i.rbegin();
    }
    inline friend const_reverse_iterator rbegin(const index& i) noexcept {
        return i.rbegin();
    }
    inline friend const_reverse_iterator crbegin(const index& i) noexcept {
        return i.crbegin();
    }

    //-----------------------------------------------------
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }
    //-----------------------------------------------------
    inline friend reverse_iterator rend(index& i) noexcept {
        return i.rend();
    }
    inline friend const_reverse_iterator rend(const index& i) noexcept {
        return i.rend();
    }
    inline friend const_reverse_iterator crend(const index& i) noexcept {
        return i.crend();
    }

    //-----------------------------------------------------
    template<class Ostream>
    inline friend Ostream&
    operator << (Ostream& os, const index<n,T>& idx)
    {
        for(auto i = size_type(0); i < n-1; ++i) {
            os << static_cast<std::int_least64_t>(idx.m_[i]) << ' ';
        }
        os << static_cast<std::int_least64_t>(idx.m_[n-1]);

        return os;
    }

private:
    //---------------------------------------------------------------
    /// @brief initialization via static int array
    template<int... ints>
    constexpr
    index(std::integer_sequence<int,ints...>): m_{ints...} {}

    value_type m_[n];
};




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<std::size_t i, std::size_t n, class T>
inline constexpr T&
get(index<n,T>& idx) noexcept
{
    static_assert(i < n, "get<i>(index<n,T>) : (i >= n) out of bounds!");

    return idx.m_[i];
}


//-------------------------------------------------------------------
template<std::size_t i, std::size_t n, class T>
inline constexpr const T&
get(const index<n,T>& idx) noexcept
{
    static_assert(i < n, "get<i>(index<n,T>) : (i >= n) out of bounds!");

    return idx.m_[i];
}



/*************************************************************************//***
 *
 * @brief index equality
 *
 *****************************************************************************/
template<typename T, std::size_t n>
inline bool
operator == (const index<n,T>& a, const index<n,T>& b) {
    return std::equal(a.begin(), a.end(), b.begin());
}


//-------------------------------------------------------------------
template<typename T, std::size_t n>
inline bool
operator != (const index<n,T>& a, const index<n,T>& b) {
    return !(a == b);
}



/*************************************************************************//***
 *
 * @brief lexicographic comparison
 *
 *****************************************************************************/
template<typename T, std::size_t n>
inline bool
operator < (const index<n,T>& a, const index<n,T>& b) {
    for(std::size_t i = 0; i < n; ++i) if(a[i] >= b[i]) return false;
    return true;
}

template<typename T, std::size_t n>
inline bool
operator > (const index<n,T>& a, const index<n,T>& b) {
    for(std::size_t i = 0; i < n; ++i) if(a[i] <= b[i]) return false;
    return true;
}

template<typename T, std::size_t n>
inline bool
operator <= (const index<n,T>& a, const index<n,T>& b) {
    for(std::size_t i = 0; i < n; ++i) if(a[i] > b[i]) return false;
    return true;
}

template<typename T, std::size_t n>
inline bool
operator >= (const index<n,T>& a, const index<n,T>& b) {
    for(std::size_t i = 0; i < n; ++i) if(a[i] < b[i]) return false;
    return true;
}



/*************************************************************************//***
 *
 * @brief swap index elements
 *
 *****************************************************************************/
template<typename T, std::size_t n>
inline void
swap(index<n,T>& a, index<n,T>& b) noexcept {
    a.swap(b);
}



/*************************************************************************//***
 *
 * @brief
 *
 *****************************************************************************/
template<class T>
inline constexpr index<1,T>
operator + (const index<1,T>& a, const index<1,T>& b) noexcept {
    return index<1,T>{ a[0] + b[0] };
}

template<class T>
inline constexpr index<1,T>&&
operator + (index<1,T>&& a, const index<1,T>& b) noexcept {
    a[0] += b[0];
    return std::move(a);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr index<2,T>
operator + (const index<2,T>& a, const index<2,T>& b) noexcept {
    return index<2,T>{ a[0] + b[0], a[1] + b[1] };
}

template<class T>
inline constexpr index<2,T>&&
operator + (index<2,T>&& a, const index<2,T>& b) noexcept {
    a[0] += b[0];
    a[1] += b[1];
    return std::move(a);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr index<3,T>
operator + (const index<3,T>& a, const index<3,T>& b) noexcept {
    return index<3,T>{ a[0] + b[0], a[1] + b[1], a[2] + b[2] };
}

template<class T>
inline constexpr index<3,T>&&
operator + (index<3,T>&& a, const index<3,T>& b) noexcept {
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
    return std::move(a);
}


//-------------------------------------------------------------------
template<typename T, std::size_t n>
inline index<n,T>
operator + (const index<n,T>& a, const index<n,T>& b) noexcept {
    index<n,T> c;
    for(std::size_t i = 0; i < n; ++i) c[i] = a[i] + b[i];
    return c;
}

template<typename T, std::size_t n>
inline index<n,T>&&
operator + (index<n,T>&& a, const index<n,T>& b) noexcept {
    for(std::size_t i = 0; i < n; ++i) a[i] += b[i];
    return std::move(a);
}



/*************************************************************************//***
 *
 * @brief
 *
 *****************************************************************************/
template<class T>
inline constexpr index<1,T>
operator + (const index<1,T>& a, T b) noexcept {
    return index<1,T>{ a[0] + b };
}

template<class T>
inline constexpr index<1,T>&&
operator + (index<1,T>&& a, T b) noexcept {
    a[0] += b;
    return std::move(a);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr index<2,T>
operator + (const index<2,T>& a, T b) noexcept {
    return index<2,T>{ a[0] + b, a[1] + b };
}

template<class T>
inline constexpr index<2,T>&&
operator + (index<2,T>&& a, T b) noexcept {
    a[0] += b;
    a[1] += b;
    return std::move(a);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr index<3,T>
operator + (const index<3,T>& a, T b) noexcept {
    return index<3,T>{ a[0] + b, a[1] + b, a[2] + b };
}

template<class T>
inline constexpr index<3,T>&&
operator + (index<3,T>&& a, T b) noexcept {
    a[0] += b;
    a[1] += b;
    a[2] += b;
    return std::move(a);
}


//-------------------------------------------------------------------
template<typename T, std::size_t n>
inline index<n,T>
operator + (const index<n,T>& a, T b) noexcept {
    index<n,T> c;
    for(std::size_t i = 0; i < n; ++i) c[i] = a[i] + b;
    return c;
}

template<typename T, std::size_t n>
inline index<n,T>&&
operator + (index<n,T>&& a, T b) noexcept {
    for(std::size_t i = 0; i < n; ++i) a[i] += b;
    return std::move(a);
}



/*************************************************************************//***
 *
 * @brief
 *
 *****************************************************************************/
template<class T>
inline constexpr index<1,T>
operator - (const index<1,T>& a, const index<1,T>& b) noexcept {
    return index<1,T>{ a[0] - b[0] };
}

template<class T>
inline constexpr index<1,T>&&
operator - (index<1,T>&& a, const index<1,T>& b) noexcept {
    a[0] -= b[0];
    return std::move(a);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr index<2,T>
operator - (const index<2,T>& a, const index<2,T>& b) noexcept {
    return index<2,T>{ a[0] - b[0], a[1] - b[1] };
}

template<class T>
inline constexpr index<2,T>&&
operator - (index<2,T>&& a, const index<2,T>& b) noexcept {
    a[0] -= b[0];
    a[1] -= b[1];
    return std::move(a);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr index<3,T>
operator - (const index<3,T>& a, const index<3,T>& b) noexcept {
    return index<3,T>{ a[0] - b[0], a[1] - b[1], a[2] - b[2] };
}

template<class T>
inline constexpr index<3,T>&&
operator - (index<3,T>&& a, const index<3,T>& b) noexcept {
    a[0] -= b[0];
    a[1] -= b[1];
    a[2] -= b[2];
    return std::move(a);
}


//-------------------------------------------------------------------
template<typename T, std::size_t n>
inline index<n,T>
operator - (const index<n,T>& a, const index<n,T>& b) noexcept {
    index<n,T> c;
    for(std::size_t i = 0; i < n; --i) c[i] = a[i] - b[i];
    return c;
}

template<typename T, std::size_t n>
inline index<n,T>&&
operator - (index<n,T>&& a, const index<n,T>& b) noexcept {
    for(std::size_t i = 0; i < n; --i) a[i] -= b[i];
    return std::move(a);
}



/*************************************************************************//***
 *
 * @brief
 *
 *****************************************************************************/
template<class T>
inline constexpr index<1,T>
operator - (const index<1,T>& a, T b) noexcept {
    return index<1,T>{ a[0] - b };
}

template<class T>
inline constexpr index<1,T>&&
operator - (index<1,T>&& a, T b) noexcept {
    a[0] -= b;
    return std::move(a);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr index<2,T>
operator - (const index<2,T>& a, T b) noexcept {
    return index<2,T>{ a[0] - b, a[1] - b };
}

template<class T>
inline constexpr index<2,T>&&
operator - (index<2,T>&& a, T b) noexcept {
    a[0] -= b;
    a[1] -= b;
    return std::move(a);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr index<3,T>
operator - (const index<3,T>& a, T b) noexcept {
    return index<3,T>{ a[0] - b, a[1] - b, a[2] - b };
}

template<class T>
inline constexpr index<3,T>&&
operator - (index<3,T>&& a, T b) noexcept {
    a[0] -= b;
    a[1] -= b;
    a[2] -= b;
    return std::move(a);
}


//-------------------------------------------------------------------
template<typename T, std::size_t n>
inline index<n,T>
operator - (const index<n,T>& a, T b) noexcept {
    index<n,T> c;
    for(std::size_t i = 0; i < n; ++i) c[i] = a[i] - b;
    return c;
}

template<typename T, std::size_t n>
inline index<n,T>&&
operator - (index<n,T>&& a, T b) noexcept {
    for(std::size_t i = 0; i < n; ++i) a[i] -= b;
    return std::move(a);
}



/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<std::size_t n, class T>
inline constexpr index<n,T>
range_size(const index<n,T>& a, const index<n,T>& b)
{
    return (b - a) + 1;
}



}  // namespace am




namespace std {

/*************************************************************************//***
 *
 * @brief make std::tuple_size work with 'index'
 *
 *****************************************************************************/
template<size_t n, class T>
struct tuple_size<am::index<n,T>> :
    public integral_constant<size_t,n>
{};

}  // namespace std


#endif
