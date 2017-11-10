/******************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2012-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_CONTAINERS_COMPRESSED_MULTISET_H_
#define AMLIB_CONTAINERS_COMPRESSED_MULTISET_H_

#include <map>
#include <iterator>


namespace am {


/*************************************************************************//***
 *
 * @brief stores a set of representatives of equivalence classes
 *
 * @details The interface is modeled after std::multiset.
 *     Instead of storing all inserted values only one representative of
 *     any equivalence (not equality!) class is stored.
 *     Equivalence is defined by the Compare functor.
 *
 * @tparam Value    type of values/representatives to be stored
 * @tparam Compare  used to determine value equivalence
 *
 *****************************************************************************/
template<
    class Value,
    class Compare = std::less<Value>,
    class Allocator = std::allocator<std::pair<const Value,std::size_t>>
>
class compressed_multiset
{
    using count_t = std::size_t;
    using mem_t = std::map<Value,count_t,Compare,Allocator>;
    using mem_iter_t  = typename mem_t::iterator;
    using mem_citer_t = typename mem_t::const_iterator;

    //-----------------------------------------------------
    template<class Iter>
    struct iterator_t
    {
        friend class compressed_multiset;

        //-----------------------------------------------------
        using value_type = Value;
        using reference = const value_type&;
        using pointer = const value_type*;
        using difference_type = typename std::iterator_traits<Iter>::difference_type;
        using iterator_category = std::bidirectional_iterator_tag;

        //-----------------------------------------------------
        constexpr
        iterator_t() noexcept : it_(), n_(0) {}

        constexpr
        iterator_t(Iter it, count_t n = count_t(0)) noexcept : it_(it), n_(n) {}

        //-----------------------------------------------------
        constexpr reference
        operator * () const noexcept {
            return it_->first;
        }
        //-----------------------------------------------------
        constexpr pointer
        operator -> () const noexcept {
            return std::addressof(it_->first);
        }

        //-----------------------------------------------------
        iterator_t&
        operator ++ () noexcept {
            ++n_;
            if(n_ >= it_->second) {
                n_ = count_t(0);
                ++it_;
            }
            return *this;
        }
        //-----------------------------------------------------
        iterator_t&
        operator -- () noexcept {
            if(n_ < count_t(1)) {
                --it_;
                n_ = it_->second;
            }
            --n_;
            return *this;
        }
        //-----------------------------------------------------
        iterator_t
        operator ++ (int) noexcept {
            iterator_t old(*this);
            ++*this;
            return old;
        }
        //-----------------------------------------------------
        iterator_t
        operator -- (int) noexcept {
            iterator_t old(*this);
            --*this;
            return old;
        }
        //-----------------------------------------------------
        iterator_t&
        operator += (difference_type d) noexcept {
            if(d < difference_type(0))
                return operator-=(-d);

            d += difference_type(n_);
            while(d > difference_type(0) && d > difference_type(it_->second)) {
                d -= difference_type(it_->second);
                ++it_;
            }
            n_ = d;
            return *this;
        }
        //-----------------------------------------------------
        iterator_t&
        operator -= (difference_type d) noexcept {
            if(d < difference_type(0))
                return operator+=(-d);

            if(d > difference_type(n_)) {
                --it_;
                d -= difference_type(n_);
            }

            while(d > difference_type(0) && d > difference_type(it_->second)) {
                --it_;
                d -= difference_type(it_->second);
            }
            n_ = it_->second - d;
            return *this;
        }
        //-----------------------------------------------------
        iterator_t&
        operator + (difference_type d) noexcept {
            auto it = *this;
            it += d;
            return it;
        }
        //-----------------------------------------------------
        iterator_t&
        operator - (difference_type d) noexcept {
            auto it = *this;
            it -= d;
            return it;
        }

        //---------------------------------------------------------------
        bool operator ==(const iterator_t& other) noexcept {
            return (it_ == other.it_ && n_ == other.n_);
        }
        bool operator !=(const iterator_t& other) noexcept {
            return (it_ != other.it_ || n_ != other.n_);
        }

        //---------------------------------------------------------------
        inline friend difference_type
        distance(const iterator_t& a, const iterator_t& b) {
            using std::next;

            if(a == b) return difference_type(0);

            auto dist = difference_type(a.it_->second - a.n_ + b.n_);
            for(auto it = next(a.it_); it != b.it_; ++it) {
                dist += it->second;
            }
            return dist;
        }

    private:
        Iter it_;
        count_t n_;
    };


    //-----------------------------------------------------
    template<class Iter>
    struct unique_iter_t
    {
        friend class compressed_multiset;

        //-----------------------------------------------------
        using value_type = Value;
        using reference = const value_type&;
        using pointer = const value_type*;
        using difference_type = typename std::iterator_traits<Iter>::difference_type;
        using iterator_category = std::bidirectional_iterator_tag;

        //-----------------------------------------------------
        constexpr
        unique_iter_t() noexcept : it_() {}

        constexpr
        unique_iter_t(Iter it) noexcept : it_(it) {}

        //-----------------------------------------------------
        constexpr reference
        operator * () const noexcept {
            return it_->first;
        }
        //-----------------------------------------------------
        constexpr pointer
        operator -> () const noexcept {
            return std::addressof(it_->first);
        }

        //-----------------------------------------------------
        unique_iter_t&
        operator ++ () noexcept {
                ++it_;
            return *this;
        }
        //-----------------------------------------------------
        unique_iter_t&
        operator -- () noexcept {
            --it_;
            return *this;
        }
        //-----------------------------------------------------
        unique_iter_t&
        operator ++ (int) noexcept {
            unique_iter_t old(*this);
            ++*this;
            return old;
        }
        //-----------------------------------------------------
        unique_iter_t&
        operator -- (int) noexcept {
            unique_iter_t old(*this);
            --*this;
            return old;
        }
        //-----------------------------------------------------
        unique_iter_t&
        operator += (difference_type d) noexcept {
            it_ -= d;
            return *this;
        }
        //-----------------------------------------------------
        unique_iter_t&
        operator -= (difference_type d) noexcept {
            it_ -= d;
            return *this;
        }
        //-----------------------------------------------------
        unique_iter_t&
        operator + (difference_type d) noexcept {
            auto it = *this;
            it += d;
            return it;
        }
        //-----------------------------------------------------
        unique_iter_t&
        operator - (difference_type d) noexcept {
            auto it = *this;
            it -= d;
            return it;
        }

        //---------------------------------------------------------------
        bool operator ==(const unique_iter_t& other) noexcept {
            return (it_ == other.it_);
        }
        bool operator !=(const unique_iter_t& other) noexcept {
            return (it_ != other.it_);
        }

        //---------------------------------------------------------------
        inline friend difference_type
        distance(const unique_iter_t& a, const unique_iter_t& b) {
            using std::distance;
            return distance(a.it_,b.it_);
        }

    private:
        Iter it_;
    };


public:
    //---------------------------------------------------------------
    using value_type     = Value;
    using value_compare  = Compare;
    using key_compare    = Compare;
    using allocator_type = Allocator;
    using size_type      = std::size_t;
    using const_iterator = iterator_t<mem_citer_t>;
    using iterator       = const_iterator;
    //-----------------------------------------------------
    using const_unique_iterator = unique_iter_t<mem_citer_t>;
    using unique_iterator       = const_unique_iterator;


    //---------------------------------------------------------------
    compressed_multiset():
        size_(0), mem_()
    {}
    //-----------------------------------------------------
    explicit
    compressed_multiset(const key_compare& keycomp,
                        const allocator_type& alloc = allocator_type{}):
        size_(0), mem_(keycomp,alloc)
    {}
    //-----------------------------------------------------
    explicit
    compressed_multiset(const allocator_type& alloc):
        size_(0), mem_(alloc)
    {}
    //-----------------------------------------------------
    explicit
    compressed_multiset(std::initializer_list<value_type> il):
        size_(0), mem_()
    {
        assign(il);
    }
    //-----------------------------------------------------
    explicit
    compressed_multiset(std::initializer_list<std::pair<value_type,size_type>> il):
        size_(0), mem_()
    {
        assign(il);
    }

    //-----------------------------------------------------
    compressed_multiset(const compressed_multiset&) = default;
    compressed_multiset(compressed_multiset&&) = default;


    //---------------------------------------------------------------
    compressed_multiset& operator = (const compressed_multiset&) = default;
    compressed_multiset& operator = (compressed_multiset&&) = default;


    //---------------------------------------------------------------
    void
    assign(std::initializer_list<value_type> il)
    {
        assign(il.begin(), il.end());
    }
    //-----------------------------------------------------
    void
    assign(std::initializer_list<std::pair<value_type,size_type>> il)
    {
        mem_.clear();
        for(const auto& x : il) {
            insert(x.first, x.second);
        }
    }
    //-----------------------------------------------------
    template<class InputIterator>
    void
    assign(InputIterator first, InputIterator last)
    {
        mem_.clear();
        for(; first != last; ++first) {
            insert(*first);
        }
    }


    //---------------------------------------------------------------
    constexpr size_type
    size() const noexcept {
        return size_;
    }
    //-----------------------------------------------------
    size_type
    max_size() const noexcept {
        return mem_.max_size();
    }
    //-----------------------------------------------------
    size_type
    unique() const noexcept {
        return mem_.size();
    }
    //-----------------------------------------------------
    bool
    empty() const noexcept {
        return mem_.empty();
    }


    //---------------------------------------------------------------
    const_iterator
    find(const value_type& v) const {
        return const_iterator(mem_.find(v));
    }
    //---------------------------------------------------------------
    const_iterator
    lower_bound(const value_type& v) const {
        return const_iterator(mem_.lower_bound(v));
    }
    //---------------------------------------------------------------
    const_iterator
    upper_bound(const value_type& v) const {
        return const_iterator(mem_.upper_bound(v));
    }
    //---------------------------------------------------------------
    std::pair<const_iterator,const_iterator>
    equal_range(const value_type& v) const {
        return std::make_pair(lower_bound(v), upper_bound(v));
    }
    //-----------------------------------------------------
    size_type
    count(const value_type& v) const {
        const auto it = mem_.find(v);
        return (it != mem_.end()) ? it->second : size_type(0);
    }


    //---------------------------------------------------------------
    const_iterator
    insert(const value_type& v, size_type n = 1) {
        if(n < size_type(1)) return const_iterator(mem_.end());

        auto it = mem_.find(v);
        //equivalent value already in map -> increase counter
        if(it != mem_.end()) {
            it->second += n;
            size_ += n;
            return const_iterator(it);
        }
        //insert new unique value
        auto ip = mem_.insert(std::make_pair(v,size_type(n)));
//        if(ip.second)
        size_ += n;
        return const_iterator(ip.first);
    }
    //-----------------------------------------------------
    const_iterator
    insert(value_type&& v, size_type n = 1) {
        if(n < size_type(1)) return const_iterator(mem_.end());

        auto it = mem_.find(v);
        //equivalent value already in map -> increase counter
        if(it != mem_.end()) {
            it->second += n;
            size_ += n;
            return const_iterator(it);
        }
        //insert new unique value
        auto ip = mem_.insert(std::make_pair(std::move(v),size_type(n)));
//        if(ip.second)
        size_ += n;
        return const_iterator(ip.first);
    }

    //-----------------------------------------------------
    void
    insert(std::initializer_list<std::pair<value_type,size_t>> il) {
        for(const auto& x : il) {
            insert(x.first, x.second);
        }
    }
    //-----------------------------------------------------
    void
    insert(std::initializer_list<value_type> il) {
        for(const auto& x : il) {
            insert(x);
        }
    }


    //---------------------------------------------------------------
    const_iterator
    erase(const_iterator it)
    {
        using std::prev;
        if(it.it_ != mem_.end()) {
            const auto n = it.it_->second;

            if(n == 1) {
                auto pos = mem_.erase(it.it_);
                --size_;
                return const_iterator(pos);
            }
            else if(n > 1) {
                auto mit = mem_.find(*it);
                --(mit->second);
                --size_;
            }
        }
        return it;
    }
    //-----------------------------------------------------
    const_iterator
    erase(const_iterator first, const_iterator last)
    {
        if(first == last) return erase(first);

        const auto n = distance(first,last);

        auto fst = first.it_;
        auto lst = last.it_;

        //decrease count in first bucket?
        auto offset = first.n_;
        if(first.n_ > count_t(0)) {
            auto mit = mem_.find(*fst);
            mit->second -= mit->second - first.n_;
            ++fst;
        }
        //decrease count in last bucket?
        if(last.n_ > count_t(0)) {
            auto mit = mem_.find(*lst);
            mit->second -= last.n_;
        }

        auto pos = mem_.erase(fst,lst);
        if(offset < count_t(1)) offset = pos->second-1;

        return const_iterator(pos, offset);
    }
    //-----------------------------------------------------
    size_type
    erase(const value_type& v) {
        auto it = mem_.find(v);
        if(it == mem_.end()) return size_type(0);
        const auto m = it->second;
        mem_.erase(it);
        size_ -= m;
        return m;
    }
    //-----------------------------------------------------
    size_type
    erase(const value_type& v, size_type n) {
        auto it = mem_.find(v);
        //none found
        if(it != mem_.end()) {
            if(n >= it->second) {
                const auto m = it->second;
                mem_.erase(it);
                size_ -= m;
                return m;
            } else {
                it->second -= n;
                size_ -= n;
                return n;
            }
        }
        return size_type(0);
    }


    //---------------------------------------------------------------
    void clear() {
        mem_.clear();
        size_ = size_type(0);
    }


    //---------------------------------------------------------------
    value_compare
    value_comp() const noexcept {
        return mem_.value_comp();
    }
    //-----------------------------------------------------
    key_compare
    key_comp() const noexcept {
        return mem_.key_comp();
    }

    //-----------------------------------------------------
    allocator_type
    get_allocator() const {
        return mem_.get_allocator();
    }


    //---------------------------------------------------------------
    const_iterator
    begin() const noexcept {
        return const_iterator(mem_.begin());
    }
    const_iterator
    cbegin() const noexcept {
        return const_iterator(mem_.cbegin());
    }
    //-----------------------------------------------------
    const_iterator
    end() const noexcept {
        return const_iterator(mem_.end());
    }
    const_iterator
    cend() const noexcept {
        return const_iterator(mem_.cend());
    }


    //---------------------------------------------------------------
    const_unique_iterator
    begin_unique() const noexcept {
        return const_unique_iterator(mem_.begin());
    }
    const_unique_iterator
    cbegin_unique() const noexcept {
        return const_unique_iterator(mem_.cbegin());
    }
    //-----------------------------------------------------
    const_unique_iterator
    end_unique() const noexcept {
        return const_unique_iterator(mem_.end());
    }
    const_unique_iterator
    cend_unique() const noexcept {
        return const_unique_iterator(mem_.cend());
    }


private:
    size_type size_;
    mem_t mem_;

};







/******************************************************************************
 *
 *
 *
 *
 *****************************************************************************/
template<class T, class C, class A>
inline auto
begin(const compressed_multiset<T,C,A>& r) noexcept -> decltype(r.begin()) {
    return r.begin();
}

//---------------------------------------------------------
template<class T, class C, class A>
inline auto
cbegin(const compressed_multiset<T,C,A>& r) noexcept -> decltype(r.cbegin()) {
    return r.cbegin();
}



//-------------------------------------------------------------------
template<class T, class C, class A>
inline auto
end(const compressed_multiset<T,C,A>& r) noexcept -> decltype(r.end()) {
    return r.end();
}

//---------------------------------------------------------
template<class T, class C, class A>
inline auto
cend(const compressed_multiset<T,C,A>& r) noexcept -> decltype(r.cend()) {
    return r.cend();
}



}  // namespace am


#endif
