/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2015 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_CONTAINERS_CRS_MATRIX_H_
#define AMLIB_CONTAINERS_CRS_MATRIX_H_

#include <type_traits>
#include <iterator>
#include <memory>
#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>


namespace am {


/*****************************************************************************
 *
 * @brief wrapper for a compile time value
 *
 *****************************************************************************/
template<class T, int val>
struct static_value {
    static constexpr T value() { return T(val); }
};


/*****************************************************************************
 *
 * @brief
 *
 *****************************************************************************/
template<class T>
struct default_value {
    static T value() { return T(); }
};



/*****************************************************************************
 *
 * @brief compressed row storage sparse matrix
 *
 * @tparam ValueType  content type
 * @tparam NAvalue    determines the (static) value to be returned for
 *                    "empty" elements (usually zero for numeric matrices)
 * @tparam Allocator
 *
 * @details internal representation of (N x M) matrix:
 *          [value1,            ..., valueM              ] : M
 *          [col_index(value1), ..., col_index(valueM)   ] : M
 *          [start_of_row1, ..., start_of_rowN, #elements] : N+1
 *
 *****************************************************************************/
template<
    class ValueType,
    class NAvalue = static_value<ValueType,0>,
    class Allocator = std::allocator<ValueType>
>
class crs_matrix
{
    using value_vector = std::vector<ValueType,Allocator>;
    using index_vector  = std::vector<typename value_vector::size_type,Allocator>;


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
    using na_value_type   = NAvalue;
    using allocator_type  = Allocator;
    //-----------------------------------------------------
    using reference       = typename value_vector::reference;
    using const_reference = typename value_vector::const_reference;
    //-----------------------------------------------------
    using pointer         = typename value_vector::pointer;
    using const_pointer   = typename value_vector::const_pointer;
    //-----------------------------------------------------
    using size_type       = typename value_vector::size_type;
    //-----------------------------------------------------
    using iterator        = typename value_vector::iterator;
    using const_iterator  = typename value_vector::const_iterator;
    //-----------------------------------------------------
    using index_iterator  = typename index_vector::const_iterator;
    //-----------------------------------------------------
    using row_range       = iter_range_t_<iterator,size_type>;
    using const_row_range = iter_range_t_<const_iterator,size_type>;
    using index_range     = iter_range_t_<index_iterator,typename index_vector::size_type>;


    //---------------------------------------------------------------
    // CONSTRUCTION / DESTRUCTION
    //---------------------------------------------------------------
    /** @brief  default constructor
     */
    crs_matrix():
        values_{},
        colidx_{},
        rowbeg_{0}
    {}

    //-----------------------------------------------------
    /** @brief  constructs (1 x N) matrix with values from initializer list
     */
    crs_matrix(std::initializer_list<value_type> il):
        values_(il),
        colidx_(values_.size()),
        rowbeg_{0, values_.size()}
    {
        std::iota(colidx_.begin(), colidx_.end(), size_type(0));
    }

    //-----------------------------------------------------
    /** @brief  constructs (1 x N) matrix with values from vector (copying)
     */
    crs_matrix(const value_vector& v):
        values_(v),
        colidx_(values_.size()),
        rowbeg_{0, values_.size()}
    {
        std::iota(colidx_.begin(), colidx_.end(), size_type(0));
    }

    //-----------------------------------------------------
    /** @brief  constructs (1 x N) matrix with values from vector (moving)
     */
    crs_matrix(value_vector&& v):
        values_(std::move(v)),
        colidx_(values_.size()),
        rowbeg_{0, values_.size()}
    {
        std::iota(colidx_.begin(), colidx_.end(), size_type(0));
    }

    //-----------------------------------------------------
    /** @brief  constructs (1 x N) matrix with values from iterator range
     */
    template<class InputIterator, class InputEndIterator = InputIterator>
    crs_matrix(InputIterator first, InputEndIterator last):
        values_(first,last),
        colidx_(values_.size()),
        rowbeg_{0, values_.size()}
    {
        std::iota(colidx_.begin(), colidx_.end(), size_type(0));
    }


    //---------------------------------------------------------------
    crs_matrix(const crs_matrix&) = default;
    crs_matrix(crs_matrix&&) = default;


    //---------------------------------------------------------------
    crs_matrix& operator = (const crs_matrix&) = default;
    crs_matrix& operator = (crs_matrix&&) = default;


    //---------------------------------------------------------------
    void
    reserve(size_type numElements)
    {
        values_.reserve(numElements);
        colidx_.reserve(numElements);
    }
    //-----------------------------------------------------
    void
    reserve_rows(size_type rows)
    {
        rowbeg_.reserve(rows+1);
    }


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    /** @brief  => (1 x N) matrix with values from iterator range
     */
    template<class InputIterator, class InputEndIterator>
    void assign(InputIterator first, InputEndIterator last)
    {
        values_.assign(first,last);

        colidx_.resize(values_.size());
        std::iota(colidx_.begin(), colidx_.end(), size_type(0));

        rowbeg_.clear();
        rowbeg_.push_back(0, values_.size());
    }

    //-----------------------------------------------------
    /** @brief  => (1 x N) matrix with values from vector (copy)
     */
    void assign(const value_vector& v)
    {
        values_ = v;

        colidx_.resize(values_.size());
        std::iota(colidx_.begin(), colidx_.end(), size_type(0));

        rowbeg_.clear();
        rowbeg_.push_back(0, values_.size());
    }

    //-----------------------------------------------------
    /** @brief  => (1 x N) matrix with values from vector (move)
     */
    void assign(value_vector&& v)
    {
        values_ = std::move(v);

        colidx_.resize(values_.size());
        std::iota(colidx_.begin(), colidx_.end(), size_type(0));

        rowbeg_.clear();
        rowbeg_.push_back(0, values_.size());
    }


    //---------------------------------------------------------------
    // RESTRUCTURING
    //---------------------------------------------------------------
    /** @brief  split values into rows with given sizes
     */
    void
    row_sizes(std::initializer_list<size_type> sizes)
    {
        row_sizes(sizes.begin(), sizes.end());
    }
    //-----------------------------------------------------
    /** @brief  split values into rows with given sizes
     */
    template<class InputIterator, class InputEndIterator = InputIterator>
    void
    row_sizes(InputIterator first, InputEndIterator last)
    {
        rowbeg_.clear();
        rowbeg_.push_back(0);
        auto ci = colidx_.begin();
        size_type i = *first;
        for(; first != last && i < values_.size(); ) {
            std::iota(ci, ci + *first, size_type(0));
            rowbeg_.push_back(i);
            ci += *first;
            ++first;
            i += *first;
        }
        std::iota(ci, colidx_.end(), size_type(0));
        rowbeg_.push_back(values_.size());
    }



    //---------------------------------------------------------------
    /** @brief  sets the column indices of elements within one row
     *          The number of indices has to be the same as the number
     *          of row elements. The input indices have to be sorted in
     *          ascending order for the function to succeed.
     *
     * @return  true if the column indices were modified, false otherwise
     */
    bool
    col_indices(size_type row, std::initializer_list<size_type> sizes)
    {
        return col_indices(row, sizes.begin(), sizes.end());
    }

    //-----------------------------------------------------
    /** @brief  sets the column indices of elements within one row
     *          The number of indices has to be the same as the number
     *          of row elements.
     *
     * @return  true if the column indices were modified, false otherwise
     */
    template<class InputIterator, class InputEndIterator = InputIterator>
    bool
    col_indices(size_type row, InputIterator first, InputEndIterator last)
    {
        //the number of column indices has to be
        //equal to the number of elements in the row
        using std::distance;
        const auto n = distance(first, last);
        if(n != row_size(row)) return false;
        if(n < 0) return true;

        //the input values have to be sorted in ascending order
        for(auto i = first, j = i+1; j != last; ++i, ++j) {
            if(*i >= *j) return false;
        }

        //copy input values
        for(auto i = colidx_.begin() + rowbeg_[row],
                 e = colidx_.begin() + rowbeg_[row+1]; i != e; ++i, ++first)
        {
            *i = *first;
        }
        return true;
    }


    //-----------------------------------------------------
    /** @brief shift column indices of all elements in a row
     */
    void
    shift_row(size_type row, int by = 1)
    {
        if(!row_in_range(row) || by == 0) return;

        for(auto i = colidx_.begin() + rowbeg_[row],
                 e = colidx_.begin() + rowbeg_[row+1]; i != e; ++i)
        {
            *i += by;
        }
    }


    //---------------------------------------------------------------
    // ERASE
    //---------------------------------------------------------------
    void
    clear() {
        values_.clear();
        colidx_.clear();
        rowbeg_.erase(rowbeg_.begin()+1, rowbeg_.end());
        rowbeg_.front() = size_type(0);
    }

    //-----------------------------------------------------
    /**
     * @brief   erase one element
     * @return  iterator to the next element after the one that was erased
     */
    const_iterator
    erase(const_iterator it)
    {
        using std::distance;
        const auto c = distance(values_.begin(), it);

        colidx_.erase(colidx_.begin() + c);

        //decrease row begins
        for(auto p = std::lower_bound(rowbeg_.begin(), rowbeg_.end(), c);
            p != rowbeg_.end(); ++p)
        {
            --(*p);
        }

        return values_.erase(it);
    }
    //-----------------------------------------------------
    /**
     * @brief   erase one element
     * @return  true if an element was erased, false otherwise
     */
    bool
    erase(size_type row, size_type col)
    {
        auto it = find(row,col);
        if(it != end()) {
            erase(it);
            return true;
        }
        return false;
    }

    //-----------------------------------------------------
    /**
     * @brief   erase entire row
     * @return  true if one or more elements were erased, false otherwise
     */
    bool
    erase_row(size_type row)
    {
        return erase_rows(row,row);
    }

    //-----------------------------------------------------
    /**
     * @brief   erase mutliple rows
     * @return  true if one or more elements were erased, false otherwise
     */
    bool
    erase_rows(size_type first, size_type last)
    {
        if(first > last) return false;

        if(!row_in_range(first)) return false;
        if(!row_in_range(last))  return false;

        ++last;
        const auto n = rowbeg_[last] - rowbeg_[first];

        if(n < 1) return false;

        values_.erase(values_.begin() + rowbeg_[first],
                      values_.begin() + rowbeg_[last]);

        colidx_.erase(colidx_.begin() + rowbeg_[first],
                      colidx_.begin() + rowbeg_[last]);

        rowbeg_.erase(rowbeg_.begin() + first, rowbeg_.begin() + last);

        for(size_type i = first; i < rowbeg_.size(); ++i) {
            rowbeg_[i] -= n;
        }

        return true;
    }


    //---------------------------------------------------------------
    // ASSIGN / INSERT VALUE
    //---------------------------------------------------------------
    /**
     * @return  a pair:
     *     an iterator to the stored value
     *     and a bool that is
     *         true, if a new value has been inserted
     *         false, if a value (that was already present) was modified
     */
    std::pair<iterator,bool>
    insert(size_type row, size_type col, const value_type& val)
    {
        //add row(s) at the end
        if(row + 1 >= rowbeg_.size()) {
            values_.push_back(val);
            colidx_.push_back(col);
            auto add = (row + 2) - rowbeg_.size();
            rowbeg_.reserve(rowbeg_.size() + add);
            rowbeg_.insert(rowbeg_.end(), add-1, rowbeg_.back());
            rowbeg_.push_back(rowbeg_.back() + 1);
            return std::pair<iterator,bool>{values_.end()-1,true};
        }

        const auto cb = colidx_.begin() + rowbeg_[row];
        const auto ce = colidx_.begin() + rowbeg_[row+1];

        auto it = values_.begin();
        //value already in matrix
        if(cb != ce && col == *cb) {
            it += rowbeg_[row];
            *it = val;
            return std::pair<iterator,bool>{it,false};
        }
        //new value first in the new row or row empty
        else if(col < *cb || cb == ce) {
            it += rowbeg_[row];
            colidx_.insert(cb, col);
        }
        //new value last in the row?
        else if(col > *(ce-1)) {
            it += rowbeg_[row+1];
            colidx_.insert(ce, col);
        }
        //value within row
        else {
            it += rowbeg_[row];
            //find column offset
            const auto cit = std::lower_bound(cb, ce, col);
            it += distance(cb,cit);

            //value at (row,col) already in matrix => modify
            if(cit != ce && *cit == col) {
                *it = val;
                return std::pair<iterator,bool>{it,false};
            }
            //not found =! insert col index for new value
            colidx_.insert(cit, col);
        }
        //insert new value
        it = values_.insert(it, val);

        //increase row begin for all rows > row
        for(auto i = rowbeg_.begin() + row + 1; i != rowbeg_.end(); ++i)
            ++(*i);

        return std::pair<iterator,bool>{it,true};
    }

    //-----------------------------------------------------
    /**
      * @return a pair:
      *     an iterator to the stored value
      *     and a bool that is
      *         true, if a new value has been inserted
      *         false, if a value (that was already present) was modified
      */
    std::pair<iterator,bool>
    insert(size_type row, size_type col, value_type&& val)
    {
        //add row(s) at the end
        if(row + 1 >= rowbeg_.size()) {
            values_.push_back(std::move(val));
            colidx_.push_back(col);
            auto add = (row + 2) - rowbeg_.size();
            rowbeg_.reserve(rowbeg_.size() + add);
            rowbeg_.insert(rowbeg_.end(), add-1, rowbeg_.back());
            rowbeg_.push_back(rowbeg_.back() + 1);
            return std::pair<iterator,bool>{values_.end()-1,true};
        }

        const auto cb = colidx_.begin() + rowbeg_[row];
        const auto ce = colidx_.begin() + rowbeg_[row+1];

        auto it = values_.begin();
        //value already in matrix
        if(cb != ce && col == *cb) {
            it += rowbeg_[row];
            *it = std::move(val);
            return std::pair<iterator,bool>{it,false};
        }
        //new value first in the new row or row empty
        else if(col < *cb || cb == ce) {
            it += rowbeg_[row];
            colidx_.insert(cb, col);
        }
        //new value last in the row?
        else if(col > *(ce-1)) {
            it += rowbeg_[row+1];
            colidx_.insert(ce, col);
        }
        //value within row
        else {
            it += rowbeg_[row];
            //find column offset
            const auto cit = std::lower_bound(cb, ce, col);
            it += distance(cb,cit);

            //value at (row,col) already in matrix => modify
            if(cit != ce && *cit == col) {
                *it = std::move(val);
                return std::pair<iterator,bool>{it,false};
            }
            //not found =! insert col index for new value
            colidx_.insert(cit, col);
        }
        //insert new value
        it = values_.insert(it, std::move(val));

        //increase row begin for all rows > row
        for(auto i = rowbeg_.begin() + row + 1; i != rowbeg_.end(); ++i)
            ++(*i);

        return std::pair<iterator,bool>{it,true};

    }


    //---------------------------------------------------------------
    // N/A VALUE
    //---------------------------------------------------------------
    /** @return the "not available" value (usually 0 in case of numbers)
     */
    static constexpr value_type
    na_value() noexcept {
        return na_value_type::value();
    }


    //---------------------------------------------------------------
    // MATRIX ELEMENT ACCESS
    //---------------------------------------------------------------
    /** @return true, if value at (row,col) is stored, false otherwise
     */
    bool
    has(size_type row, size_type col) const {
        return offset(row,col) < size();
    }

    //-----------------------------------------------------
    /** @return iterator to value at (row,col), end() otherwise
     */
    iterator
    find(size_type row, size_type col)
    {
        return values_.begin() + offset(row,col);
    }
    //-----------------------------------------------------
    /** @return iterator to value at (row,col), end() otherwise
     */
    const_iterator
    find(size_type row, size_type col) const
    {
        return values_.begin() + offset(row,col);
    }


    //---------------------------------------------------------------
    /** @return value at (row,col) if stored, n/a-value otherwise
     */
    value_type
    operator () (size_type row, size_type col) const noexcept
    {
        auto o = offset(row,col);
        return (o < values_.size()) ? values_[o] : na_value();
    }
    //---------------------------------------------------------------
    /** @brief insert/modify value at (row,col)
     */
    void
    operator () (size_type row, size_type col, const value_type& val)
    {
        insert(row, col, val);
    }
    //---------------------------------------------------------------
    /** @brief insert/modify value at (row,col)
     */
    void
    operator () (size_type row, size_type col, value_type&& val)
    {
        insert(row, col, std::move(val));
    }


    //---------------------------------------------------------------
    // DIRECT ACCESS TO CRS REPRESENTATION
    //---------------------------------------------------------------
    const value_type*
    data() const noexcept {
        return values_.data();
    }
    const size_type*
    col_index_data() const noexcept {
        return colidx_.data();
    }
    const size_type*
    row_offset_data() const noexcept {
        return rowbeg_.data();
    }

    //-----------------------------------------------------
    /** @return cref to stored value at index in compressed storage
     */
    const_reference
    operator [] (size_type index) const noexcept {
        return values_[index];
    }
    //-----------------------------------------------------
    /** @return ref to stored value at index in compressed storage
     */
    reference
    operator [] (size_type index) noexcept {
        return values_[index];
    }


    //-----------------------------------------------------
    /** @return const iterator to begin of column index storage
     */
    index_iterator
    begin_col_indices() const noexcept {
        return colidx_.begin();
    }
    //-----------------------------------------------------
    index_iterator
    end_col_indices() const noexcept {
        return colidx_.end();
    }
    //-----------------------------------------------------
    index_iterator
    begin_col_indices(size_type row) const noexcept {
        return colidx_.begin() + rowbeg_[row];
    }
    //-----------------------------------------------------
    index_iterator
    end_col_indices(size_type row) const noexcept {
        return colidx_.begin() + rowbeg_[row+1];
    }
    //-----------------------------------------------------
    /** @return iterator range to column index storage
     */
    index_range
    col_indices() const noexcept {
        return index_range{colidx_.begin(), colidx_.end()};
    }


    //-----------------------------------------------------
    /** @return const iterator to begin of row offset storage
     */
    index_iterator
    begin_row_offsets() const noexcept {
        return rowbeg_.begin();
    }
    //-----------------------------------------------------
    index_iterator
    end_row_offsets() const noexcept {
        return rowbeg_.end();
    }
    //-----------------------------------------------------
    /** @return iterator range to row offsets storage
     */
    index_range
    row_offsets() const noexcept {
        return index_range{rowbeg_.begin(), rowbeg_.end()};
    }


    //---------------------------------------------------------------
    // INDEX QUERIES
    //---------------------------------------------------------------
    size_type
    col_index_of(const_iterator it) const noexcept {
        using std::distance;
        return colidx_[distance(values_.begin(), it)];
    }
    //-----------------------------------------------------
    size_type
    row_index_of(const_iterator it) const noexcept {
        using std::distance;
        const auto c = size_type(distance(values_.begin(), it));
        const auto p = std::upper_bound(rowbeg_.begin(), rowbeg_.end(), c);
        const auto r = size_type(distance(rowbeg_.begin(), p));
        return (r > 0) ? r-1 : 0;
    }
    //---------------------------------------------------------------
    std::pair<size_type,size_type>
    index_of(const_iterator it) const noexcept {
        using std::distance;
        const auto c = size_type(distance(values_.begin(), it));
        const auto p = std::upper_bound(rowbeg_.begin(), rowbeg_.end(), c);
        const auto r = size_type(distance(rowbeg_.begin(), p));
        return std::pair<size_type,size_type>{(r > 0) ? r-1 : 0, colidx_[c]};
    }


    //---------------------------------------------------------------
    // SIZE PROPERTIES
    //---------------------------------------------------------------
    size_type
    size() const noexcept {
        return values_.size();
    }
    //-----------------------------------------------------
    size_type
    max_size() const noexcept {
        return values_.max_size();
    }
    //-----------------------------------------------------
    size_type
    capacity() const noexcept {
        return values_.capacity();
    }
    //-----------------------------------------------------
    bool
    empty() const noexcept {
        return values_.empty();
    }
    //-----------------------------------------------------
    size_type
    rows() const noexcept {
        return rowbeg_.empty() ? 0 : rowbeg_.size() - 1;
    }
    //-----------------------------------------------------
    size_type
    cols() const noexcept {
        return empty()
            ? 0
            : *std::max_element(colidx_.begin(), colidx_.end()) + 1;
    }


    //---------------------------------------------------------------
    // ROW PROPERTIES
    //---------------------------------------------------------------
    size_type
    row_size(size_type row) const noexcept {
        return rowbeg_[row+1] - rowbeg_[row];
    }
    //-----------------------------------------------------
    bool
    row_empty(size_type row) const noexcept {
        return (row_size(row) < 1);
    }
    //-----------------------------------------------------
    bool
    row_in_range(size_type row) const noexcept {
        return row+1 < rowbeg_.size() && rowbeg_[row] < rowbeg_[row+1];
    }


    //---------------------------------------------------------------
    // SEQUENTIAL ITERATORS
    //---------------------------------------------------------------
    iterator
    begin() noexcept {
        return values_.begin();
    }
    const_iterator
    begin() const noexcept {
        return values_.begin();
    }
    const_iterator
    cbegin() const noexcept {
        return values_.begin();
    }
    //-----------------------------------------------------
    friend iterator
    begin(crs_matrix& m) noexcept {
        return m.begin();
    }
    friend const_iterator
    begin(const crs_matrix& m) noexcept {
        return m.begin();
    }
    friend const_iterator
    cbegin(const crs_matrix& m) noexcept {
        return m.begin();
    }

    //-----------------------------------------------------
    iterator
    end() noexcept {
        return values_.end();
    }
    const_iterator
    end() const noexcept {
        return values_.end();
    }
    const_iterator
    cend() const noexcept {
        return values_.end();
    }
    //-----------------------------------------------------
    iterator
    end(crs_matrix& m) noexcept {
        return m.end();
    }
    const_iterator
    end(const crs_matrix& m) noexcept {
        return m.end();
    }
    const_iterator
    cend(const crs_matrix& m) noexcept {
        return m.end();
    }


    //---------------------------------------------------------------
    iterator
    begin_row(size_type row) noexcept {
        return values_.begin() + rowbeg_[row];
    }
    //-----------------------------------------------------
    const_iterator
    begin_row(size_type row) const noexcept {
        return values_.begin() + rowbeg_[row];
    }
    //-----------------------------------------------------
    const_iterator
    cbegin_row(size_type row) const noexcept {
        return values_.begin() + rowbeg_[row];
    }

    //-----------------------------------------------------
    iterator
    end_row(size_type row) noexcept {
        return values_.begin() + rowbeg_[row+1];
    }
    //-----------------------------------------------------
    const_iterator
    end_row(size_type row) const noexcept {
        return values_.begin() + rowbeg_[row+1];
    }
    //-----------------------------------------------------
    const_iterator
    cend_row(size_type row) const noexcept {
        return values_.begin() + rowbeg_[row+1];
    }

    //-----------------------------------------------------
    row_range
    row(size_type row) noexcept {
        return row_range{begin_row(row), end_row(row)};
    }
    //-----------------------------------------------------
    const_row_range
    row(size_type row) const noexcept {
        return const_row_range{begin_row(row), end_row(row)};
    }
    //-----------------------------------------------------
    const_row_range
    crow(size_type row) const noexcept {
        return const_row_range{begin_row(row), end_row(row)};
    }


    //---------------------------------------------------------------
    friend void
    swap(crs_matrix& a, crs_matrix& b) noexcept {
        swap(a.values_, b.values_);
        swap(a.colidx_, b.colidx_);
        swap(a.rowbeg_, b.rowbeg_);
    }


    //---------------------------------------------------------------
    const allocator_type&
    get_allocator() const noexcept {
        return values_.get_allocator();
    }


    //---------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------
    template<class Ostream>
    inline friend Ostream&
    operator << (Ostream& os, const crs_matrix& m)
    {
        os << m.rows() << ' ' << m.cols() << ' ' << m.size() << '\n';

        for(const auto& x : m) os << x << ' ';
        os << '\n';
        for(const auto& x : m.col_indices()) os << x << ' ';
        os << '\n';
        for(const auto& x : m.row_offsets()) os << x << ' ';
        os << '\n';

        /*
        os << "v: " << m.values_.size() << " | ";
        for(const auto& x : m.values_) os << x << ' ';
        os << '\n';

        os << "c: " << m.colidx_.size() << " | ";
        for(const auto& x : m.colidx_) os << x << ' ';
        os << '\n';

        os << "r: " << m.rowbeg_.size() << " | ";
        for(const auto& x : m.rowbeg_) os << x << ' ';
        os << '\n';

        for(size_type r = 0; r < m.rows(); ++r) {
            for(size_type c = 0, cols = m.cols(); c < cols; ++c) {
                auto v = m(r,c);
                auto s = std::string{"-"};
                if(v != m.na_value()) s = std::to_string(v);
                s.insert(s.begin(), 4 - s.size(), ' ');
                os << s;
            }
            os << '\n';
        }
        */
        return os;
    }


private:
    //---------------------------------------------------------------
    /// @brief returns pointer offset for value at index (row,col)
    ///        or size() if value not in matrix
    size_type
    offset(size_type row, size_type col) const noexcept
    {
        using std::distance;

        if(!row_in_range(row)) return values_.size();

        const auto b = colidx_.begin() + rowbeg_[row];
        if(col == *b) return rowbeg_[row];
        if(col < *b)  return values_.size();

        const auto e = colidx_.begin() + rowbeg_[row+1];

        const auto it = std::lower_bound(b, e, col);
//        const auto it = std::find(b, e, col);

        return (it != e && *it == col)
            ? rowbeg_[row] + size_type(distance(b,it))
            : values_.size();
    }


    //---------------------------------------------------------------

    value_vector values_;
    index_vector colidx_;
    index_vector rowbeg_;
};



}  // namespace am


#endif
