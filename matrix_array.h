/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2016 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_CONTAINERS_MATRIX_ARRAY_H_
#define AMLIB_CONTAINERS_MATRIX_ARRAY_H_

#include <type_traits>
#include <iterator>
#include <utility>


namespace am {


/*****************************************************************************
 *
 * @brief statically sized 2-dimensional array
 *
 *
 *
 *
 *****************************************************************************/
template<
    class ValueType,
    std::size_t nrows,
    std::size_t ncols = nrows
>
class matrix_array
{
    using this_t_ = matrix_array<ValueType,nrows,ncols>;


    /*************************************************************************
     *
     *
     *************************************************************************/
    template<class T, std::size_t stride>
    class stride_iter_t_
    {
        using this_t_ = stride_iter_t_<T,stride>;

    public:
        //---------------------------------------------------------------
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename std::remove_const<T>::type;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;

        //---------------------------------------------------------------
        explicit constexpr
        stride_iter_t_(pointer p = nullptr) noexcept :
            p_(p)
        {}


        //---------------------------------------------------------------
        reference operator *() const noexcept {
            return (*p_);
        }
        //-----------------------------------------------------
        pointer operator ->() const noexcept {
            return (p_);
        }

        //-----------------------------------------------------
        value_type&
        operator [] (difference_type i) const noexcept {
            return *(p_ + (stride * i));
        }


        //---------------------------------------------------------------
        this_t_&
        operator ++ () noexcept {
            p_ += stride;
            return *this;
        }
        this_t_
        operator ++ (int) noexcept {
            this_t_ old(*this);
            ++*this;
            return old;
        }
        this_t_&
        operator += (difference_type i) noexcept {
            p_ += (stride * i);
            return *this;
        }
        this_t_
        operator + (difference_type i) const noexcept {
            return this_t_{p_ + (stride * i)};
        }
        //-----------------------------------------------------
        this_t_&
        operator -- () noexcept {
            p_ -= stride;
            return *this;
        }
        this_t_
        operator -- (int) noexcept {
            this_t_ old(*this);
            --*this;
            return old;
        }
        this_t_&
        operator -= (difference_type i) noexcept {
            p_ -= (stride * i);
            return *this;
        }
        this_t_
        operator - (difference_type i) const noexcept {
            return this_t_{p_ - (stride * i)};
        }

        //---------------------------------------------------------------
        explicit
        operator pointer () const noexcept {
            return p_;
        }

        //---------------------------------------------------------------
        bool operator ==(const this_t_& other) const noexcept {
            return (p_ == other.p_);
        }
        bool operator !=(const this_t_& other) const noexcept {
            return (p_ != other.p_);
        }
        bool operator < (const this_t_& other) const noexcept {
            return (p_ < other.p_);
        }
        bool operator <=(const this_t_& other) const noexcept {
            return (p_ <= other.p_);
        }
        bool operator > (const this_t_& other) const noexcept {
            return (p_ > other.p_);
        }
        bool operator >=(const this_t_& other) const noexcept {
            return (p_ >= other.p_);
        }

    private:
        pointer p_;
    };


    //---------------------------------------------------------------
    template<class T>
    struct rectangular_range_t_
    {
        //-----------------------------------------------------
        using value_type = typename std::remove_const<T>::type;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;

        //--------------------------------------------------------
        class iterator
        {
        public:
            //-----------------------------------------------------
            using iterator_category = std::forward_iterator_tag;
            using value_type = typename std::remove_const<T>::type;
            using pointer = T*;
            using reference = T&;
            using difference_type = std::ptrdiff_t;

            //-----------------------------------------------------
            explicit constexpr
            iterator(
                pointer p = nullptr,
                difference_type length = 0, difference_type stride = 0) noexcept
            :
                p_{p}, count_(0), length_(length), stride_(stride)
            {}


            //-----------------------------------------------------
            reference
            operator *() const noexcept {
                return (*p_);
            }
            //-----------------------------------------------------
            pointer
            operator ->() const noexcept {
                return (p_);
            }

            //-----------------------------------------------------
            iterator&
            operator ++ () noexcept {
                ++p_;
                ++count_;
                if(count_ >= length_) {
                    count_ = 0;
                    p_ += stride_;
                }
                return *this;
            }
            //-----------------------------------------------------
            iterator
            operator ++ (int) noexcept {
                auto old(*this);
                ++*this;
                return old;
            }

            //---------------------------------------------------------------
            explicit
            operator pointer () const noexcept {
                return p_;
            }

            //-----------------------------------------------------
            bool operator ==(const iterator& other) const noexcept {
                return (p_ == other.p_);
            }
            bool operator !=(const iterator& other) const noexcept {
                return (p_ != other.p_);
            }

        private:
            pointer p_;
            difference_type count_, length_, stride_;
        };


        //---------------------------------------------------------------
        explicit constexpr
        rectangular_range_t_(
            pointer pbeg = nullptr, pointer pend = nullptr,
            difference_type length = 0, difference_type stride = 0) noexcept
        :
            pbeg_{pbeg}, pend_{pend}, length_(length), stride_(stride)
        {}

        //-----------------------------------------------------
        constexpr iterator
        begin() const noexcept {
            return iterator{pbeg_, length_, stride_};
        }

        //-----------------------------------------------------
        constexpr iterator
        end() const noexcept {
            return iterator{pend_};
        }


    private:
        pointer pbeg_, pend_;
        difference_type length_, stride_;
    };


    //---------------------------------------------------------------
    /**
     * @brief range definition helper
     */
    template<class Iterator, class SizeT>
    class iter_range_t_
    {
    public:
        using iterator = Iterator;
        using value_type = typename
            std::decay<decltype(*std::declval<iterator>())>::type;

        using size_type = SizeT;

        iter_range_t_() = default;

        constexpr explicit
        iter_range_t_(iterator it) noexcept : beg_{it}, end_{it} {}

        constexpr explicit
        iter_range_t_(iterator beg, iterator end) noexcept : beg_{beg}, end_{end} {}

        constexpr iterator begin() const noexcept { return beg_; }
        constexpr iterator end()   const noexcept { return end_; }

        //---------------------------------------------------------------
        bool empty() const noexcept { return (beg_ == end_); }
        explicit operator bool() const noexcept { return !empty(); }

        bool operator == (const iter_range_t_& other) const noexcept {
            return (beg_ == other.beg_) && (end_ == other.end_);
        }
        bool operator != (const iter_range_t_& other) const noexcept {
            return !(*this == other);
        }

        size_type size() const noexcept {
            using std::distance;
            return distance(beg_, end_);
        }

    private:
        iterator beg_;
        iterator end_;
    };


public:
    //---------------------------------------------------------------
    // TYPES
    //---------------------------------------------------------------
    using value_type      = ValueType;
    //-----------------------------------------------------
    using reference       = ValueType&;
    using const_reference = const ValueType&;
    //-----------------------------------------------------
    using pointer         = ValueType*;
    using const_pointer   = const ValueType*;
    //-----------------------------------------------------
    using iterator        = pointer;
    using const_iterator  = const_pointer;
    //-----------------------------------------------------
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    //-----------------------------------------------------
    using row_iterator         = pointer;
    using const_row_iterator   = const_pointer;
    //-----------------------------------------------------
    using col_iterator         = stride_iter_t_<value_type,ncols>;
    using const_col_iterator   = stride_iter_t_<const value_type,ncols>;
    //-----------------------------------------------------
    using diag_iterator        = stride_iter_t_<value_type,ncols+1>;
    using const_diag_iterator  = stride_iter_t_<const value_type,ncols+1>;
    //-----------------------------------------------------
    using rectangular_range       = rectangular_range_t_<value_type>;
    using const_rectangular_range = rectangular_range_t_<const value_type>;
    //-----------------------------------------------------
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    //-----------------------------------------------------
    using row_range        = iter_range_t_<row_iterator,size_type>;
    using const_row_range  = iter_range_t_<const_row_iterator,size_type>;
    //-----------------------------------------------------
    using col_range        = iter_range_t_<col_iterator,size_type>;
    using const_col_range  = iter_range_t_<const_col_iterator,size_type>;
    //-----------------------------------------------------
    using diag_range        = iter_range_t_<diag_iterator,size_type>;
    using const_diag_range  = iter_range_t_<const_diag_iterator,size_type>;


    //---------------------------------------------------------------
    // CONSTRUCTION / DESTRUCTION
    //---------------------------------------------------------------
    /// @brief default constructor
    constexpr matrix_array() = default;

    //-----------------------------------------------------
    /// @brief initializer list constructor
    matrix_array(std::initializer_list<std::initializer_list<value_type>> il):
        m_{}
    {
        size_type totSize = std::min(il.size(),nrows) *
            std::min((il.begin())->size(),ncols);

        if(totSize > 0) {
            iterator mi = begin();
            for(auto i = il.begin(), e = il.end(); i != e; ++i) {
                std::copy(i->begin(), i->end(), mi);
                mi += ncols;
            }
        }
    }

    //-----------------------------------------------------
    /// @brief copy constructor
    matrix_array(const matrix_array&) = default;

    //-----------------------------------------------------
    /// @brief move constructor
    matrix_array(matrix_array&&) = default;

    //---------------------------------------------------------------
    ~matrix_array() = default;


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    matrix_array& operator = (const matrix_array&) = default;
    matrix_array& operator = (matrix_array&&) = default;

    //---------------------------------------------------------------
    void
    fill(const value_type& value) {
        std::fill(begin(), end(), value);
    }
    //-----------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    void
    fill_diag(const value_type& value) {
        std::fill(begin_diag(), end_diag(), value);
    }
    //-----------------------------------------------------
    void
    fill_row(size_type index, const value_type& value)
    {
        std::fill(begin_row(index), end_row(index), value);
    }
    //-----------------------------------------------------
    void
    fill_col(size_type index, const value_type& value)
    {
        std::fill(begin_col(index), end_col(index), value);
    }


    //---------------------------------------------------------------
    // ALTER DATA
    //---------------------------------------------------------------
    void
    swap_rows(size_type r1, size_type r2) {
        std::swap_ranges(begin_row(r1), end_row(r1), begin_row(r2));
    }
    //-----------------------------------------------------
    void
    swap_cols(size_type c1, size_type c2) {
        std::swap_ranges(begin_col(c1), end_col(c1), begin_col(c2));
    }


    //---------------------------------------------------------------
    // ACCESS
    //---------------------------------------------------------------
    pointer
    data() noexcept {
        return static_cast<pointer>(m_);
    }
    //-----------------------------------------------------
    const_pointer
    data() const noexcept {
        return static_cast<const value_type*>(m_);
    }
    //-----------------------------------------------------
    reference
    operator () (size_type row, size_type col) noexcept {
        return m_[row][col];
    }
    //-----------------------------------------------------
    const_reference
    operator () (size_type row, size_type col) const noexcept {
        return m_[row][col];
    }

    //-----------------------------------------------------
    row_range
    operator [] (size_type index) noexcept {
        return row_range{begin_row(index), end_row(index)};
    }
    //-----------------------------------------------------
    const_row_range
    operator [] (size_type index) const noexcept {
        return const_row_range{begin_row(index), end_row(index)};
    }


    //---------------------------------------------------------------
    // INDEX QUERIES
    //---------------------------------------------------------------
    size_type
    row_index_of(const_iterator it) const noexcept {
        using std::distance;
        return static_cast<size_type>(distance(begin(), it)) / ncols;
    }
    //-----------------------------------------------------
    size_type
    col_index_of(const_iterator it) const noexcept {
        using std::distance;
        return static_cast<size_type>(distance(begin(), it)) % ncols;
    }
    //---------------------------------------------------------------
    std::pair<size_type,size_type>
    index_of(const_iterator i) const noexcept {
        using std::distance;

        const auto n = static_cast<size_type>(distance(begin(), i));
        const auto r = static_cast<size_type>(n / ncols);

        return {r, static_cast<size_type>(n-(r*ncols))};
    }


    //---------------------------------------------------------------
    // SIZE PROPERTIES
    //---------------------------------------------------------------
    static constexpr size_type
    rows() noexcept {
        return nrows;
    }
    static constexpr size_type
    cols() noexcept {
        return ncols;
    }
    //-----------------------------------------------------
    static constexpr size_type
    size() noexcept {
        return (nrows*ncols);
    }
    //-----------------------------------------------------
    static constexpr size_type
    max_size() noexcept {
        return (nrows*ncols);
    }
    //-----------------------------------------------------
    static constexpr
    bool
    is_square() noexcept {
        return (nrows == ncols);
    }


    //---------------------------------------------------------------
    // SEQUENTIAL ITERATORS
    //---------------------------------------------------------------
    iterator
    begin() noexcept {
        return iterator{std::addressof(m_[0][0])};
    }
    const_iterator
    begin() const noexcept {
        return const_iterator{std::addressof(m_[0][0])};
    }
    const_iterator
    cbegin() const noexcept {
        return const_iterator{std::addressof(m_[0][0])};
    }
    //-----------------------------------------------------
    inline friend iterator
    begin(this_t_& i) noexcept {
        return i.begin();
    }
    inline friend const_iterator
    begin(const this_t_& i) noexcept {
        return i.begin();
    }
    inline friend const_iterator
    cbegin(const this_t_& i) noexcept {
        return i.cbegin();
    }
    //-----------------------------------------------------
    iterator
    begin(size_type row, size_type col) noexcept {
        return m_[row][col];
    }
    //-----------------------------------------------------
    const_iterator
    begin(size_type row, size_type col) const noexcept {
        return m_[row][col];
    }
    //-----------------------------------------------------
    const_iterator
    cbegin(size_type row, size_type col) const noexcept {
        return m_[row][col];
    }

    //-----------------------------------------------------
    iterator
    end() noexcept {
        return iterator{std::addressof(m_[nrows-1][ncols])};
    }
    const_iterator
    end() const noexcept    {
        return const_iterator{std::addressof(m_[nrows-1][ncols])};
    }
    const_iterator
    cend() const noexcept {
        return const_iterator{std::addressof(m_[nrows-1][ncols])};
    }
    //-----------------------------------------------------
    inline friend iterator
    end(this_t_& i) noexcept {
        return i.end();
    }
    inline friend const_iterator
    end(const this_t_& i) noexcept {
        return i.end();
    }
    inline friend const_iterator
    cend(const this_t_& i) noexcept {
        return i.cend();
    }


    //---------------------------------------------------------------
    // REVERSE SEQUENTIAL ITERATORS
    //---------------------------------------------------------------
    reverse_iterator
    rbegin() noexcept {
        return reverse_iterator(end());
    }
    const_reverse_iterator
    rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator
    crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    //-----------------------------------------------------
    inline friend reverse_iterator
    rbegin(this_t_& i) noexcept {
        return i.rbegin();
    }
    inline friend const_reverse_iterator
    rbegin(const this_t_& i) noexcept {
        return i.rbegin();
    }
    inline friend const_reverse_iterator
    crbegin(const this_t_& i) noexcept {
        return i.crbegin();
    }

    //-----------------------------------------------------
    reverse_iterator
    rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator
    rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator
    crend() const noexcept {
        return const_reverse_iterator(begin());
    }
    //-----------------------------------------------------
    inline friend reverse_iterator
    rend(this_t_& i) noexcept {
        return i.rend();
    }
    inline friend const_reverse_iterator
    rend(const this_t_& i) noexcept {
        return i.rend();
    }
    inline friend const_reverse_iterator
    crend(const this_t_& i) noexcept {
        return i.crend();
    }


    //---------------------------------------------------------------
    // ROW ITERATORS
    //---------------------------------------------------------------
    row_iterator
    begin_row(size_type row) noexcept {
        return row_iterator{std::addressof(m_[row][0])};
    }
    //-----------------------------------------------------
    const_row_iterator
    begin_row(size_type row) const noexcept {
        return const_row_iterator{std::addressof(m_[row][0])};
    }
    //-----------------------------------------------------
    const_row_iterator
    cbegin_row(size_type row) const noexcept {
        return const_row_iterator{std::addressof(m_[row][0])};
    }

    //-----------------------------------------------------
    row_iterator
    end_row(size_type row) noexcept {
        return row_iterator{std::addressof(m_[row][ncols])};
    }
    //-----------------------------------------------------
    const_row_iterator
    end_row(size_type row) const noexcept {
        return const_row_iterator{std::addressof(m_[row][ncols])};
    }
    //-----------------------------------------------------
    const_row_iterator
    cend_row(size_type row) const noexcept {
        return const_row_iterator{std::addressof(m_[row][ncols])};
    }

    //-----------------------------------------------------
    row_range
    row(size_type index) noexcept {
        return row_range{begin_row(index), end_row(index)};
    }
    //-----------------------------------------------------
    const_row_range
    row(size_type index) const noexcept {
        return const_row_range{begin_row(index), end_row(index)};
    }
    //-----------------------------------------------------
    const_row_range
    crow(size_type index) const noexcept {
        return const_row_range{begin_row(index), end_row(index)};
    }


    //---------------------------------------------------------------
    // COLUMN ITERATORS
    //---------------------------------------------------------------
    col_iterator
    begin_col(size_type col) noexcept {
        return col_iterator{std::addressof(m_[0][col])};
    }
    //-----------------------------------------------------
    const_col_iterator
    begin_col(size_type col) const noexcept {
        return const_col_iterator{std::addressof(m_[0][col])};
    }
    //-----------------------------------------------------
    const_col_iterator
    cbegin_col(size_type col) const noexcept {
        return const_col_iterator{std::addressof(m_[0][col])};
    }

    //-----------------------------------------------------
    col_iterator
    end_col(size_type col) noexcept {
        return col_iterator{std::addressof(m_[nrows][col])};
    }
    //-----------------------------------------------------
    const_col_iterator
    end_col(size_type col) const noexcept {
        return const_col_iterator{std::addressof(m_[nrows][col])};
    }
    //-----------------------------------------------------
    const_col_iterator
    cend_col(size_type col) const noexcept {
        return const_col_iterator{std::addressof(m_[nrows][col])};
    }

    //-----------------------------------------------------
    col_range
    col(size_type index) noexcept {
        return col_range{begin_col(index), end_col(index)};
    }
    //-----------------------------------------------------
    const_col_range
    col(size_type index) const noexcept {
        return const_col_range{begin_col(index), end_col(index)};
    }
    //-----------------------------------------------------
    const_col_range
    ccol(size_type index) const noexcept {
        return const_col_range{begin_col(index), end_col(index)};
    }


    //---------------------------------------------------------------
    // DIAGONAL ITERATORS
    //---------------------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    diag_iterator
    begin_diag() noexcept {
        return diag_iterator{std::addressof(m_[0][0])};
    }
    //-----------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    const_diag_iterator
    begin_diag() const noexcept {
        return const_diag_iterator{std::addressof(m_[0][0])};
    }
    //-----------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    const_diag_iterator
    begin_cdiag() const noexcept {
        return const_diag_iterator{std::addressof(m_[0][0])};
    }

    //-----------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    diag_iterator
    end_diag() noexcept {
        return diag_iterator{std::addressof(m_[nrows][ncols])};
    }
    //-----------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    const_diag_iterator
    end_diag() const noexcept {
        return const_diag_iterator{std::addressof(m_[nrows][ncols])};
    }
    //-----------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    const_diag_iterator
    end_cdiag() const noexcept {
        return const_diag_iterator{std::addressof(m_[nrows][ncols])};
    }

    //-----------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    diag_range
    diag() noexcept {
        return diag_range{begin_diag(), end_diag()};
    }
    //-----------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    const_diag_range
    diag() const noexcept {
        return const_diag_range{begin_diag(), end_diag()};
    }
    //-----------------------------------------------------
    template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
    const_diag_range
    cdiag() const noexcept {
        return const_diag_range{begin_diag(), end_diag()};
    }


    //---------------------------------------------------------------
    // SECTIONS
    //---------------------------------------------------------------
    rectangular_range
    rectangle(
        size_type firstRow, size_type firstCol,
        size_type lastRow,  size_type lastCol) noexcept
    {
        const auto stride = difference_type(ncols -  lastCol - 1 + firstCol);

        return rectangular_range{
            std::addressof(m_[firstRow][firstCol]),
            std::addressof(m_[lastRow][lastCol]) + stride + 1,
            difference_type(lastCol - firstCol + 1),
            stride };
    }
    //-----------------------------------------------------
    const_rectangular_range
    rectangle(
        size_type firstRow, size_type firstCol,
        size_type lastRow,  size_type lastCol) const noexcept
    {
        const auto stride = difference_type(ncols -  lastCol - 1 + firstCol);

        return const_rectangular_range{
            std::addressof(m_[firstRow][firstCol]),
            std::addressof(m_[lastRow][lastCol]) + stride + 1,
            difference_type(lastCol - firstCol + 1),
            stride };
    }
    //-----------------------------------------------------
    const_rectangular_range
    crectangle(
        size_type firstRow, size_type firstCol,
        size_type lastRow,  size_type lastCol) const noexcept
    {
        const auto stride = difference_type(ncols -  lastCol - 1 + firstCol);

        return const_rectangular_range{
            std::addressof(m_[firstRow][firstCol]),
            std::addressof(m_[lastRow][lastCol]) + stride + 1,
            difference_type(lastCol - firstCol + 1),
            stride };
    }


private:

    //---------------------------------------------------------------
    // DATA MEMBERS
    //---------------------------------------------------------------
    value_type m_[nrows][ncols];
};








/*****************************************************************************
 *
 *
 *
 *****************************************************************************/

//---------------------------------------------------------------
// I/O
//---------------------------------------------------------------
template<class Ostream, class T, std::size_t r, std::size_t c>
inline Ostream&
operator << (Ostream& os, const matrix_array<T,r,c>& m)
{
    return print(os, m);
}

//-------------------------------------------------------------------
template<class Ostream, class T, std::size_t nrows, std::size_t ncols>
inline Ostream&
print(Ostream& os, const matrix_array<T,nrows,ncols>& m)
{
    os << nrows <<' '<< ncols << '\n';
    if(nrows < 1 || ncols < 1) return os;

    for(std::size_t r = 0; r < nrows-1; ++r) {
        for(std::size_t c = 0; c < ncols-1; ++c) {
            os << m(r,c) << ' ';
        }
        os << m(r,ncols-1) << '\n';
    }
    for(std::size_t c = 0; c < ncols; ++c) {
        os << m(nrows-1,c) << ' ';
    }
    return os;
}


}  // namespace am


#endif
