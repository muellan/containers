/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_CONTAINERS_TRIANGLE_MATRIX_H_
#define AMLIB_CONTAINERS_TRIANGLE_MATRIX_H_

#include <type_traits>
#include <iterator>
#include <memory>
#include <vector>
#include <algorithm>
#include <utility>

#include <iostream>


namespace am {


/*************************************************************************//***
 *
 * @brief    lower triangle matrix with
 *           row index interval:    [1, n]    (n rows)
 *           column index interval: [0, n-1]  (n rows)
 *
 * @details  triangle_matrix elements (row index, col index) for n = 4:
 *           (1,0)
 *           (2,0) (2,1)
 *           (3,0) (3,1) (3,2)
 *           (4,0) (4,1) (4,2) (4,3)
 *
 * @tparam   ValueType: content type
 * @tparam   Allocator:
 *
 *****************************************************************************/
template<
    class ValueType,
    class Allocator = std::allocator<ValueType>
>
class triangle_matrix
{
    using alloc_traits = std::allocator_traits<Allocator>;


    /*************************************************************************
     *
     *
     *************************************************************************/
    template<class T>
    class col_iter_t_
    {
        friend class triangle_matrix;
    public:
        //---------------------------------------------------------------
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename std::remove_const<T>::type;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;

        //---------------------------------------------------------------
        explicit constexpr
        col_iter_t_() noexcept :
            p_(nullptr), stride_{0}
        {}

    private:
        //---------------------------------------------------------------
        explicit constexpr
        col_iter_t_(pointer p, std::size_t stride = 0) noexcept :
            p_(p), stride_{static_cast<difference_type>(stride)}
        {}

    public:

        //---------------------------------------------------------------
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
        value_type&
        operator [] (difference_type i) const noexcept {
            return *(p_ - ( ((stride_ + i + 1) * (stride_ + i - 2)) / 2 ));
        }


        //---------------------------------------------------------------
        col_iter_t_&
        operator ++ () noexcept {
            p_ += stride_;
            ++stride_;
            return *this;
        }
        col_iter_t_
        operator ++ (int) noexcept {
            col_iter_t_ old{*this};
            ++*this;
            return old;
        }
        col_iter_t_&
        operator += (difference_type i) noexcept {
            p_ += -(((stride_ + i + 1) * (stride_ + i - 2)) / 2);
            return *this;
        }
        col_iter_t_
        operator + (difference_type i) const noexcept {
            return col_iter_t_{p_ -(((stride_ + i + 1) * (stride_ + i - 2)) / 2),
                           stride_ + i};
        }
        //-----------------------------------------------------
        col_iter_t_&
        operator -- () noexcept {
            p_ -= stride_ - 1;
            return *this;
        }
        col_iter_t_
        operator -- (int) noexcept {
            col_iter_t_ old{*this};
            --*this;
            return old;
        }
        col_iter_t_&
        operator -= (difference_type i) noexcept {
            p_ += i * (i + 1 - (2 * stride_));
            return *this;
        }
        col_iter_t_
        operator - (difference_type i) const noexcept {
            return col_iter_t_{p_ + (i * (i + 1 - (2 * stride_))), stride_};
        }


        //---------------------------------------------------------------
        explicit
        operator pointer () const noexcept {
            return p_;
        }

        //---------------------------------------------------------------
        bool operator ==(const col_iter_t_& other) const noexcept {
            return (p_ == other.p_);
        }
        bool operator !=(const col_iter_t_& other) const noexcept {
            return (p_ != other.p_);
        }
        bool operator < (const col_iter_t_& other) const noexcept {
            return (p_ < other.p_);
        }
        bool operator <=(const col_iter_t_& other) const noexcept {
            return (p_ <= other.p_);
        }
        bool operator > (const col_iter_t_& other) const noexcept {
            return (p_ > other.p_);
        }
        bool operator >=(const col_iter_t_& other) const noexcept {
            return (p_ >= other.p_);
        }

    private:
        pointer p_;
        difference_type stride_;
    };



    /*************************************************************************
     *
     *
     *************************************************************************/
    template<class T>
    class index_iter_t_
    {
        friend class triangle_matrix;
    public:
        //---------------------------------------------------------------
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename std::remove_const<T>::type;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;

        //---------------------------------------------------------------
        explicit constexpr
        index_iter_t_() noexcept :
            p_(nullptr), px_(nullptr), stride_{0}
        {}

    private:
        //---------------------------------------------------------------
        explicit constexpr
        index_iter_t_(pointer p, pointer px = nullptr,
                      difference_type stride = 0) noexcept
        :
            p_(p), px_(px), stride_{stride}
        {}

    public:
        //---------------------------------------------------------------
        reference
        operator * () const noexcept {
            return (*p_);
        }
        //-----------------------------------------------------
        pointer
        operator -> () const noexcept {
            return (p_);
        }

        //---------------------------------------------------------------
        index_iter_t_&
        operator ++ () noexcept {
            if(p_ < px_) {
                ++p_;
            }
            else if(stride_ < 0) {
                stride_ = -stride_;
                p_ += stride_;
            }
            else {
                p_ += stride_;
                ++stride_;
            }

            return *this;
        }

        index_iter_t_
        operator ++ (int) noexcept {
            index_iter_t_ old{*this};
            ++*this;
            return old;
        }

        //---------------------------------------------------------------
        explicit
        operator pointer () const noexcept {
            return p_;
        }

        //---------------------------------------------------------------
        bool operator ==(const index_iter_t_& other) const noexcept {
            return (p_ == other.p_);
        }
        bool operator !=(const index_iter_t_& other) const noexcept {
            return (p_ != other.p_);
        }
        bool operator < (const index_iter_t_& other) const noexcept {
            return (p_ < other.p_);
        }
        bool operator <=(const index_iter_t_& other) const noexcept {
            return (p_ <= other.p_);
        }
        bool operator > (const index_iter_t_& other) const noexcept {
            return (p_ > other.p_);
        }
        bool operator >=(const index_iter_t_& other) const noexcept {
            return (p_ >= other.p_);
        }

    private:
        pointer p_;
        pointer px_;
        difference_type stride_;
    };



    /*************************************************************************
     *
     *
     *************************************************************************/
    template<class Pointer>
    struct range_t_
    {
        friend class triangle_matrix;

        //-----------------------------------------------------
        struct iterator
        {
            using size_type = std::size_t;
            using traits_t_ = std::iterator_traits<Pointer>;

            friend class range_t_;
            friend class triangle_matrix;

        public:
            //-------------------------------------------
            using iterator_category = std::forward_iterator_tag;
            using value_type = typename traits_t_::value_type;
            using pointer = typename traits_t_::pointer;
            using reference = typename traits_t_::reference;
            using difference_type = typename traits_t_::difference_type;

            //-------------------------------------------
            explicit constexpr
            iterator(Pointer p) noexcept :
                p_{p}, mark_{p}, stride_(0), n_(0)
            {}

        private:
            //-------------------------------------------
            explicit constexpr
            iterator(Pointer p, size_type fidx, size_type lidx) noexcept :
                p_{p},
                mark_{p_ + (lidx > 1
                        ? (((lidx*(lidx+1)/2) - ((fidx*(fidx-1)/2))) - 1)
                        : 0) },
                stride_(lidx > 0 ? -fidx-1 : 1), n_(lidx-fidx)
            {}

        public:
            //-------------------------------------------
            iterator&
            operator ++ () noexcept {
                if(p_ < mark_) {
                    ++p_;
                }
                else {
                    if(stride_ < 0) {
                       stride_ = -stride_;
                       p_ += stride_;
                       mark_ += stride_ + n_;
                    }
                    else {
                        p_ += stride_;
                        mark_ += stride_ + n_;
                        ++stride_;
                    }
                }
                return *this;
            }
            //-------------------------------------------
            iterator
            operator ++ (int) noexcept {
                iterator old(*this);
                ++*this;
                return old;
            }

            //-------------------------------------------
            auto
            operator * () const noexcept
                -> decltype(*std::declval<Pointer>())
            {
                return *p_;
            }
            //-------------------------------------------
            auto
            operator -> () const noexcept
                -> decltype(std::addressof(*std::declval<Pointer>()))
            {
                return std::addressof(*p_);
            }

            //---------------------------------------------------------------
            explicit
            operator pointer () const noexcept {
                return p_;
            }

            //-------------------------------------------
            bool operator == (const iterator& other) const noexcept {
                return (p_ == other.p_);
            }
            bool operator != (const iterator& other) const noexcept {
                return (p_ != other.p_);
            }

        private:
            Pointer p_;
            Pointer mark_;
            difference_type stride_;
            difference_type n_;
        };


        //-----------------------------------------------------
        using difference_type = typename iterator::difference_type;
        using size_type = typename iterator::size_type;


        //-----------------------------------------------------
        constexpr
        range_t_():
            fst_(nullptr), lst_(nullptr), fidx_(0), lidx_(0)
        {}

    private:
        //-----------------------------------------------------
        explicit constexpr
        range_t_(Pointer beg, Pointer lst,
                   size_type fidx, size_type lidx) :
            fst_{beg}, lst_{lst}, fidx_{fidx}, lidx_{lidx}
        {}

    public:

        //-----------------------------------------------------
        constexpr iterator
        begin() const {
            return iterator(fst_, fidx_, lidx_);
        }
        //-----------------------------------------------------
        constexpr iterator
        end() const {
            return iterator(lst_);
        }


    private:
        Pointer fst_, lst_;
        size_type fidx_, lidx_;
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
    using value_type     = ValueType;
    using allocator_type = Allocator;
    //-----------------------------------------------------
    using reference       = ValueType&;
    using const_reference = const ValueType&;
    //-----------------------------------------------------
    using pointer         = ValueType*;
    using const_pointer   = const ValueType*;
    //-----------------------------------------------------
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    //-----------------------------------------------------
    using iterator        = pointer;
    using const_iterator  = const_pointer;
    //-----------------------------------------------------
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    //-----------------------------------------------------
    using row_iterator       = pointer;
    using const_row_iterator = const_pointer;
    //-----------------------------------------------------
    using col_iterator       = col_iter_t_<value_type>;
    using const_col_iterator = col_iter_t_<const value_type>;
    //-----------------------------------------------------
    using index_iterator       = index_iter_t_<value_type>;
    using const_index_iterator = index_iter_t_<const value_type>;
    //-----------------------------------------------------
    using row_range        = iter_range_t_<row_iterator,size_type>;
    using const_row_range  = iter_range_t_<const_row_iterator,size_type>;
    //-----------------------------------------------------
    using col_range        = iter_range_t_<col_iterator,size_type>;
    using const_col_range  = iter_range_t_<const_col_iterator,size_type>;
    //-----------------------------------------------------
    using index_range       = iter_range_t_<index_iterator,size_type>;
    using const_index_range = iter_range_t_<const_index_iterator,size_type>;
    //-----------------------------------------------------
    using range       = range_t_<pointer>;
    using const_range = range_t_<const_pointer>;


    //---------------------------------------------------------------
    // CONSTRUCTION / DESTRUCTION
    //---------------------------------------------------------------
    /// @brief default constructor
    explicit constexpr
    triangle_matrix():
        rows_{0},
        first_{nullptr}, last_{nullptr}, memEnd_{nullptr},
        alloc_{}
    {}

    //-----------------------------------------------------
    /// @brief copy constructor
    /// @details delegates to special private constructor
    triangle_matrix(const triangle_matrix& o):
        triangle_matrix(o, o.size())
    {}

    //-----------------------------------------------------
    /// @brief move constructor
    triangle_matrix(triangle_matrix&& o) noexcept :
        rows_{o.rows_},
        first_{o.first_}, last_{o.last_}, memEnd_{o.memEnd_},
        alloc_{o.alloc_}
    {
        o.rows_ = 0;
        o.first_ = nullptr;
        o.last_ = nullptr;
        o.memEnd_ = nullptr;
    }

    //---------------------------------------------------------------
    ~triangle_matrix() {
        mem_erase();
    }


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    triangle_matrix&
    operator = (const triangle_matrix& source) {
        if(this != &source) {
            triangle_matrix temp{source};
            swap(*this,temp);
        }
        return *this;
    }
    //-----------------------------------------------------
    triangle_matrix&
    operator = (triangle_matrix&& source) noexcept {
        if(this != &source) {
            swap(source, *this);
        }
        return *this;
    }

    //---------------------------------------------------------------
    void
    fill(const value_type& value)
    {
        std::fill(begin(), end(), value);
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
    //-----------------------------------------------------
    void
    fill_at(size_type index, const value_type& value)
    {
        std::fill(begin_at(index), end_at(index), value);
    }


    //---------------------------------------------------------------
    // SET SIZE
    //---------------------------------------------------------------
    void
    rows(size_type n)
    {
        mem_resize_rows(n);
    }
    //-----------------------------------------------------
    void
    rows(size_type n, const value_type& value)
    {
        mem_resize_rows(n, value);
    }

    //-----------------------------------------------------
    void
    reserve_rows(size_type rows)
    {
        auto newSize = mem_size(rows);

         //grow capacity if needed (copies old values to new memory)
        if(newSize > capacity()) {
            auto temp = triangle_matrix(*this, newSize);
            swap(*this, temp);
        }
    }


    //---------------------------------------------------------------
    // INSERT
    //---------------------------------------------------------------
    void
    insert_at(size_type index, const value_type& value)
    {
        mem_insert_at(index, size_type(1), value);
    }

    //-----------------------------------------------------
    void
    insert_at(size_type index, size_type quantity, const value_type& value)
    {
        mem_insert_at(index, quantity, value);
    }


    //---------------------------------------------------------------
    // REMOVE
    //---------------------------------------------------------------
    void
    erase_at(size_type index) {
        mem_erase_at(index, size_type(1));
    }
    //-----------------------------------------------------
    void
    erase_at(size_type firstIndexIncl, size_type lastIndexIncl) {
        mem_erase_at(firstIndexIncl, lastIndexIncl - firstIndexIncl + 1);
    }
    //-----------------------------------------------------
    void
    erase_last() {
        mem_erase_last_row();
    }

    //-----------------------------------------------------
    void
    clear() {
        rows_ = 0;
        mem_destroy_content();
    }


    //---------------------------------------------------------------
    // ACCESS
    //---------------------------------------------------------------
    reference
    operator () (size_type row, size_type col) noexcept {
        return *ptr(row, col);
    }
    //-----------------------------------------------------
    const_reference
    operator () (size_type row, size_type col) const noexcept {
        return *ptr(row, col);
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
    // SIZE PROPERTIES
    //---------------------------------------------------------------
    size_type
    rows() const noexcept {
        return rows_;
    }
    size_type
    cols() const noexcept {
        return rows_;
    }
    //-----------------------------------------------------
    size_type
    size() const noexcept {
        using std::distance;
        return distance(first_, last_);
    }
    //-----------------------------------------------------
    size_type
    max_size() const noexcept {
        return alloc_traits::max_size(alloc_);
    }
    //-----------------------------------------------------
    size_type
    capacity() const noexcept {
        using std::distance;
        return distance(first_, memEnd_);
    }
    //-----------------------------------------------------
    bool
    empty() const noexcept {
        return (first_ == last_);
    }


    //---------------------------------------------------------------
    // INDEX QUERIES
    //---------------------------------------------------------------
    std::pair<size_type,size_type>
    index_of(const_iterator it) const noexcept
    {
        if(it == first_) return {size_type(1), size_type(0)};

        auto p = first_ + 1;
        size_type r = 2;

        for(; r <= rows_; ++r) {
            if(it >= p && it < (p + r))
                return {r, size_type(it - p)};

            p += r;
        }

        //invalid index
        return {size_type(0), size_type(0)};
    }


    //---------------------------------------------------------------
    // SEQUENTIAL ITERATORS
    //---------------------------------------------------------------
    iterator
    begin() noexcept {
        return first_;
    }
    const_iterator
    begin() const noexcept {
        return first_;
    }
    const_iterator
    cbegin() const noexcept {
        return first_;
    }
    //-----------------------------------------------------
    friend iterator
    begin(triangle_matrix& m) noexcept {
        return m.first_;
    }
    friend const_iterator
    begin(const triangle_matrix& m) noexcept {
        return m.first_;
    }
    friend const_iterator
    cbegin(const triangle_matrix& m) noexcept {
        return m.first_;
    }
    //-----------------------------------------------------
    iterator
    begin(size_type row, size_type col) noexcept {
        return ptr(row,col);
    }
    //-----------------------------------------------------
    const_iterator
    begin(size_type row, size_type col) const noexcept {
        return ptr(row,col);
    }
    //-----------------------------------------------------
    const_iterator
    cbegin(size_type row, size_type col) const noexcept {
        return ptr(row,col);
    }

    //-----------------------------------------------------
    iterator
    end() noexcept {
        return last_;
    }
    const_iterator
    end() const noexcept {
        return last_;
    }
    const_iterator
    cend() const noexcept {
        return last_;
    }
    //-----------------------------------------------------
    iterator
    end(triangle_matrix& m) noexcept {
        return m.last_;
    }
    const_iterator
    end(const triangle_matrix& m) noexcept {
        return m.last_;
    }
    const_iterator
    cend(const triangle_matrix& m) noexcept {
        return m.last_;
    }


    //---------------------------------------------------------------
    // REVERSE SEQUENTIAL ITERATORS
    //---------------------------------------------------------------
    reverse_iterator
    rbegin() noexcept {
        return (last_);
    }
    const_reverse_iterator
    rbegin() const noexcept {
        return (last_);
    }
    const_reverse_iterator
    crbegin() const noexcept {
        return (last_);
    }
    //-----------------------------------------------------
    friend reverse_iterator
    rbegin(triangle_matrix& m) noexcept {
        return (m.last_);
    }
    friend const_reverse_iterator
    rbegin(const triangle_matrix& m) noexcept {
        return (m.last_);
    }
    friend const_reverse_iterator
    crbegin(const triangle_matrix& m) noexcept {
        return (m.last_);
    }

    //-----------------------------------------------------
    reverse_iterator
    rend() noexcept {
        return first_;
    }
    const_reverse_iterator
    rend() const noexcept {
        return first_;
    }
    const_reverse_iterator
    crend() const noexcept {
        return first_;
    }
    //-----------------------------------------------------
    friend reverse_iterator
    rend(triangle_matrix& m) noexcept {
        return m.first_;
    }
    friend const_reverse_iterator
    rend(const triangle_matrix& m) noexcept {
        return m.first_;
    }
    friend const_reverse_iterator
    crend(const triangle_matrix& m) noexcept {
        return m.first_;
    }


    //---------------------------------------------------------------
    // ROW ITERATORS
    //---------------------------------------------------------------
    row_iterator
    begin_row(size_type row) noexcept {
        return ptr(row,0);
    }
    //-----------------------------------------------------
    const_row_iterator
    begin_row(size_type row) const noexcept {
        return ptr(row,0);
    }
    //-----------------------------------------------------
    const_row_iterator
    cbegin_row(size_type row) const noexcept {
        return ptr(row,0);
    }

    //-----------------------------------------------------
    row_iterator
    end_row(size_type row) noexcept {
        return ptr(row+1,0);
    }
    //-----------------------------------------------------
    const_row_iterator
    end_row(size_type row) const noexcept {
        return ptr(row+1,0);
    }
    //-----------------------------------------------------
    const_row_iterator
    cend_row(size_type row) const noexcept {
        return ptr(row+1,0);
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
        return col_iterator{ptr(col+1, col), col+1};
    }
    //-----------------------------------------------------
    const_col_iterator
    begin_col(size_type col) const noexcept {
        return const_col_iterator{ptr(col+1, col), col+1};
    }
    //-----------------------------------------------------
    const_col_iterator
    cbegin_col(size_type col) const noexcept {
        return const_col_iterator{ptr(col+1, col), col+1};
    }

    //-----------------------------------------------------
    col_iterator
    end_col(size_type col) noexcept {
        return col_iterator{ptr(rows_+1, col)};
    }
    //-----------------------------------------------------
    const_col_iterator
    end_col(size_type col) const noexcept {
        return const_col_iterator{ptr(rows_+1, col)};
    }
    //-----------------------------------------------------
    const_col_iterator
    cend_col(size_type col) const noexcept {
        return const_col_iterator{ptr(rows_+1, col)};
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
    // INDEX ITERATORS
    //---------------------------------------------------------------
    index_iterator
    begin_at(size_type index) noexcept {
        using d_t = typename index_iterator::difference_type;
        auto p = ptr(index < 1 ? 1 : index, 0);
        return index_iterator{p, p + index - 1, d_t(index < 1 ? 1 : -index-1)};
    }
    //-----------------------------------------------------
    const_index_iterator
    begin_at(size_type index) const noexcept {
        using d_t = typename const_index_iterator::difference_type;
        auto p = ptr(index < 1 ? 1 : index, 0);
        return const_index_iterator{p, p + index - 1, d_t(index < 1 ? 1 : -index-1)};
    }
    //-----------------------------------------------------
    const_index_iterator
    cbegin_at(size_type index) const noexcept {
        using d_t = typename const_index_iterator::difference_type;
        auto p = ptr(index < 1 ? 1 : index, 0);
        return const_index_iterator{p, p + index - 1, d_t(index < 1 ? 1 : -index-1)};
    }

    //-----------------------------------------------------
    index_iterator
    end_at(size_type index) noexcept {
        return index_iterator{ptr(rows_ + 1, index)};
    }
    //-----------------------------------------------------
    const_index_iterator
    end_at(size_type index) const noexcept {
        return const_index_iterator{ptr(rows_ + 1, index)};
    }
    //-----------------------------------------------------
    const_index_iterator
    cend_at(size_type index) const noexcept {
        return const_index_iterator{ptr(rows_ + 1, index)};
    }

    //-----------------------------------------------------
    index_range
    index_interval(size_type i) noexcept {
        return index_range{begin_at(i), end_at(i)};
    }
    //-----------------------------------------------------
    const_index_range
    index_interval(size_type i) const noexcept {
        return const_index_range{begin_at(i), end_at(i)};
    }
    //-----------------------------------------------------
    const_index_range
    cindex_interval(size_type i) const noexcept {
        return const_index_range{begin_at(i), end_at(i)};
    }


    //---------------------------------------------------------------
    range
    index_interval(size_type firstIndex, size_type lastIndex) noexcept
    {
        return range{ptr(firstIndex < 1 ? 1 : firstIndex,0),
                       ptr(rows_+1,firstIndex),
                       firstIndex, lastIndex};
    }
    //-----------------------------------------------------
    const_range
    index_interval(size_type firstIndex, size_type lastIndex) const noexcept
    {
        return const_range{ptr(firstIndex < 1 ? 1 : firstIndex,0),
                             ptr(rows_+1,firstIndex),
                             firstIndex, lastIndex};
    }
    //-----------------------------------------------------
    const_range
    cindex_interval(size_type firstIndex, size_type lastIndex) const noexcept
    {
        return const_range{ptr(firstIndex < 1 ? 1 : firstIndex,0),
                             ptr(rows_+1,firstIndex),
                             firstIndex, lastIndex};
    }


    //---------------------------------------------------------------
    friend void
    swap(triangle_matrix& a, triangle_matrix& b) noexcept
    {
        using std::swap;

        swap(a.rows_,   b.rows_);
        swap(a.first_,  b.first_);
        swap(a.last_,   b.last_);
        swap(a.memEnd_, b.memEnd_);
        swap(a.alloc_,  b.alloc_);
    }


    //---------------------------------------------------------------
    const allocator_type&
    get_allocator() const noexcept {
        return alloc_;
    }


    //---------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------
    template<class Ostream>
    inline friend Ostream&
    operator << (Ostream& os, const triangle_matrix& m)
    {
        os << m.rows_ << '\n';
        if(m.rows_ < 1) return os;

        for(size_type r = 1; r <= m.rows(); ++r) {
            for(size_type c = 0; c < r; ++c) {
                os << m(r,c) << ' ';
            }
            os << '\n';
        }

        return os;
    }


private:
    //---------------------------------------------------------------
    explicit
    triangle_matrix(size_type rows, size_type capacity):
        rows_{rows},
        first_{nullptr}, last_{nullptr}, memEnd_{nullptr},
        alloc_{}
    {
        if(capacity > 0) {
            first_ = alloc_traits::allocate(alloc_,capacity);
            memEnd_ = first_ + capacity;
            last_ = first_;
        }
    }

    //---------------------------------------------------------------
    explicit
    triangle_matrix(const triangle_matrix& o, size_type capacity):
        rows_{o.rows_},
        first_{nullptr}, last_{nullptr}, memEnd_{nullptr},
        alloc_{alloc_traits::select_on_container_copy_construction(o.alloc_)}
    {
        if(capacity > 0) {
            first_ = alloc_traits::allocate(alloc_,capacity);
            memEnd_ = first_ + capacity;
            last_ = first_;

            for(auto s = o.first_; s < o.last_; ++s, ++last_)
                alloc_traits::construct(alloc_, last_, *s);
        }
    }

    //-----------------------------------------------------
    template<class... Args>
    explicit
    triangle_matrix(size_type rows, Args&&... args):
        rows_{rows},
        first_{nullptr}, last_{nullptr}, memEnd_{nullptr},
        alloc_{}
    {
        //initial capacity will be exactly the same as size
        auto totSize = mem_size(rows);
        if(totSize > 0) {
            //reserve memory
            first_ = alloc_traits::allocate(alloc_,totSize);
            memEnd_ = first_ + totSize;
            //default-contruct elements in-place
            for(last_ = first_; last_ < memEnd_; ++last_)
                alloc_traits::construct(alloc_, last_,
                    std::forward<Args>(args)...);
        }
    }


    //---------------------------------------------------------------
    void
    mem_destroy_content() {
        if(!first_ || !last_) return;

        --last_;
        for(; last_ >= first_; --last_) {
            alloc_traits::destroy(alloc_, last_);
        }
        ++last_;
    }


    //---------------------------------------------------------------
    template<class... Args>
    void
    mem_resize_rows(size_type rows, Args&&... args)
    {
        if(rows == rows_) return;

        if(rows == 0) {
            clear();
        }
        else {
            auto newSize = mem_size(rows);
            mem_resize(newSize, std::forward<Args>(args)...);
            rows_ = rows;
        }
    }


    //---------------------------------------------------------------
    template<class... Args>
    void
    mem_resize(size_type newSize, Args&&... args)
    {
        if(newSize < size()) {
            //destroy elements no longer needed
            for(auto e = first_ + newSize; last_ > e; --last_) {
                alloc_traits::destroy(alloc_, last_);
            }
        }
        else {
            //grow capacity if needed (copies old values to new memory)
            if(newSize > capacity()) {
                auto temp = triangle_matrix(*this, newSize);

                //(default-)construct new elements if neccessary
                for(auto e = temp.first_ + newSize;
                    temp.last_ < e; ++temp.last_)
                {
                    alloc_traits::construct(temp.alloc_, temp.last_,
                        std::forward<Args>(args)...);
                }
                swap(*this, temp);
            }
            else {
                //(default-)construct new elements if neccessary
                for(auto e = first_ + newSize; last_ < e; ++last_) {
                    alloc_traits::construct(alloc_, last_,
                        std::forward<Args>(args)...);
                }
            }
        }
    }


    //---------------------------------------------------------------
    void
    mem_erase() {
        mem_destroy_content();

        if(first_) {
            alloc_traits::deallocate(alloc_, first_, capacity());
        }

        rows_ = 0;
        first_ = nullptr;
        last_ = nullptr;
        memEnd_ = nullptr;
    }


    //---------------------------------------------------------------
    void
    mem_erase_at(size_type first, size_type quantity)
    {
        //invalid index
        if(first > rows_) {
            return;
        }
        //including last row & col
        else if(first < 1 && first + quantity >= rows_) {
            clear();
        }
        //general case
        else {
            using std::min;
            quantity = min(quantity, first < 1 ? rows_ : rows_ - first + 1);

            const auto r1 = first < 1 ? quantity + 1 : first + quantity;
            const auto r2 = first < 1 ? quantity : first + quantity;
            auto src = ptr(r1, 0);
            auto tgt = ptr(first < 1 ? 1 : first, 0);

            for(size_type row = r1; row <= rows_; ++row) {
                if(first > 0) {
                    for(size_type i = 0; i < first; ++i, ++tgt, ++src) {
                        *tgt = std::move(*src);
                    }
                }
                src += quantity;
                for(size_type i = r2; i < row; ++i, ++tgt, ++src) {
                    *tgt = std::move(*src);
                }
            }
            //destroy remaining items
            for(auto p = tgt; p < last_; ++p) {
                alloc_traits::destroy(alloc_, p);
            }
            last_ = tgt;
            rows_ -= quantity;
        }
    }


    //---------------------------------------------------------------
    void
    mem_erase_last_row()
    {
        auto lst = ptr(rows_, 0);

        //destroy remaining items
        for(auto p = lst; p < last_; ++p) {
            alloc_traits::destroy(alloc_, p);
        }

        last_ = lst;
        --rows_;
    }


    //---------------------------------------------------------------
    void
    mem_insert_at(size_type index, size_type quantity, const value_type& value)
    {
        //move old values to new places
        if(rows_ < 1 || index > rows_) {
            mem_resize_rows(rows_ + quantity, value);
        }
        else {
            const auto newSize = mem_size(rows_ + quantity);

            //allocate new memory
            if(newSize > capacity()) {
                //make new temp matrix with completely uninizialized memory
                //and copy values over
                auto temp = triangle_matrix(rows_ + quantity, newSize);
                auto src = first_;
                auto tgt = temp.first_;

                //rows above new rows
                if(index > 1) {
                    //unchanged rows above new elements
                    for(size_type r = 1; r < index; ++r) {
                        for(size_type c = 0; c < r; ++c, ++src, ++tgt) {
                            alloc_traits::construct(temp.alloc_, tgt, *src);
                        }
                    }
                }
                //new rows
                const size_type r0 = index < 2 ? 1 : index;
                for(size_type r = r0; r < r0 + quantity; ++r) {
                    for(size_type c = 0; c < r; ++c, ++tgt) {
                        alloc_traits::construct(temp.alloc_, tgt, value);
                    }
                }
                //mixed rows below new rows
                const size_type r1 = index < 2 ? quantity+1 : index + quantity;
                for(size_type r = r1; r <= temp.rows_; ++r) {
                    //columns before new elements
                    for(size_type c = 0; c < index; ++c, ++src, ++tgt) {
                        alloc_traits::construct(temp.alloc_, tgt, *src);
                    }
                    //new elements
                    for(size_type i = 0; i < quantity; ++i, ++tgt) {
                        alloc_traits::construct(temp.alloc_, tgt, value);
                    }
                    //columns after new elements
                    for(size_type c = index+quantity; c < r; ++c, ++src, ++tgt) {
                        alloc_traits::construct(temp.alloc_, tgt, *src);
                    }
                }

                temp.last_ = temp.first_ + newSize;
                //commit changes
                swap(*this, temp);
            }
            //use old memory (capacity >= newSize)
            else {
                auto src = last_ - 1;
                auto tgt = first_ + newSize - 1;

                const size_type r0 = (index < 1 ? 0 : index) + quantity;

                for(size_type r = rows_ + quantity; r >= r0; --r) {
                    for(size_type c = r; c > r0; --c, --src, --tgt) {
                        if(tgt >= last_) { //if we are in uninitialized memory
                            alloc_traits::construct(alloc_, tgt, std::move(*src));
                        } else {
                            *tgt = std::move(*src);
                        }
                    }
                    for(size_type i = 0; i < quantity; ++i, --tgt) {
                        if(tgt >= last_) { //if we are in uninitialized memory
                            alloc_traits::construct(alloc_, tgt, value);
                        } else {
                            *tgt = value;
                        }
                    }
                    for(size_type c = index; c > 0; --c, --src, --tgt) {
                        if(tgt >= last_) { //if we are in uninitialized memory
                            alloc_traits::construct(alloc_, tgt, std::move(*src));
                        } else {
                            *tgt = std::move(*src);
                        }
                    }
                }
                //overwrite old values still left in the newly inserted rows
                auto vtgt = index < 2 ? first_ : ptr(index, 0);
                for(size_type r = index; r < index + quantity; ++r) {
                    for(size_type c = 0; c < r; ++c, ++vtgt) {
                        if(vtgt >= last_) { //if we are in uninitialized memory
                            alloc_traits::construct(alloc_, vtgt, value);
                        } else {
                            *vtgt = value;
                        }
                    }
                }
                last_ = first_ + newSize;
                rows_ += quantity;
            }
        }

    }


    //---------------------------------------------------------------
    pointer
    ptr(size_type row, size_type col) const noexcept
    {
        return first_ + ( ((row - 1) * row) / 2 + col );
    }


    //---------------------------------------------------------------
    static constexpr size_type
    mem_size(size_type rows)
    {
        return (rows * (rows + 1)) / 2;
    }


    //---------------------------------------------------------------
    size_type rows_ = 0;
    pointer first_ = nullptr;
    pointer last_ = nullptr;
    pointer memEnd_ = nullptr;
    allocator_type alloc_;
};



}  // namespace am



#endif
