/*****************************************************************************
 *
 * AM containers
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_UNIFORM_HISTOGRAM_H_
#define AMLIB_UNIFORM_HISTOGRAM_H_

#include <vector>
#include <cstdint>
#include <numeric>


namespace am {


/*****************************************************************************
 *
 * @brief holds a vector of bin counts
 *        all bins have the same width
 *        insert(x) increases the count of the bin that x falls in
 *        operator()(x) returns the count of the bin that x falls in
 *
 *****************************************************************************/
template<
    class ArgumentT,
    class Bins = std::vector<std::uint_least32_t>
>
class uniform_histogram
{
public:
    //---------------------------------------------------------------
    using value_type = typename Bins::value_type;
    using count_type = value_type;
    using size_type = typename Bins::size_type;
    //-----------------------------------------------------
    using const_iterator = typename Bins::const_iterator;
    using iterator       = typename Bins::iterator;
    //-----------------------------------------------------
    using argument_type = ArgumentT;
    using numeric_type = argument_type;


    //---------------------------------------------------------------
    explicit
    uniform_histogram() :
        min_(0), max_(0), width_(0), 
        bins_()
    {}
    //-----------------------------------------------------
    explicit
    uniform_histogram(argument_type binWidth) :
        min_(0), max_(0),
        width_((binWidth > 0) ? std::move(binWidth) : argument_type(0)),
        bins_()
    {}
    //-----------------------------------------------------
    explicit
    uniform_histogram(
        argument_type min, argument_type max, argument_type binWidth)
    :
        min_(std::move(min)), max_(std::move(max)),
        width_((binWidth > 0) ? std::move(binWidth) : argument_type(0)),
        bins_()
    {
        using std::swap;
        if(min_ > max_) swap(min_,max_);
        const auto s = required_size(min_,max_,width_);
        bins_.resize(s);
        max_ = min_ + width_ * s;
    }

    //-----------------------------------------------------
    uniform_histogram(const uniform_histogram&) = default;
    uniform_histogram(uniform_histogram&&)      = default;


    //---------------------------------------------------------------
    uniform_histogram& operator = (const uniform_histogram&) = default;
    uniform_histogram& operator = (uniform_histogram&&)      = default;


    //---------------------------------------------------------------
    void
    clear() {
        for(auto& x : bins_) {
            x = 0;
        }
    }


    //---------------------------------------------------------------
    void
    expand(argument_type newMin, argument_type newMax) {
        using std::swap;

        if(newMin > newMax) swap(newMin,newMax);

        if(newMin < min_ || newMax > max_) {
            auto addLow = 0; //number of additional bins needed below min_

            if(newMin < min_) {
                addLow = required_size(newMin,min_,width_);
                newMin = min_ - addLow * width_;
            } else {
                newMin = min_;
            }
            if(newMax < max_) {
                newMax = max_;
            }
            auto newBins = Bins{};
            const auto newSize = required_size(newMin,newMax,width_);
            newBins.reserve(newSize);

            //first construct new lower bins
            newBins.resize(addLow, value_type(0));
            //copy-insert old bins
            newBins.insert(newBins.end(), bins_.begin(), bins_.end());
            //construct additional upper bins
            newBins.resize(newSize, value_type(0));

            //done
            min_ = std::move(newMin);
            max_ = min_ + width_ * newSize;
            newBins.swap(bins_);
        }
    }
    //-----------------------------------------------------
    void
    expand_include(const argument_type& value)
    {
        expand(value, value + width_);
    }


    //---------------------------------------------------------------
    const argument_type&
    min() const noexcept {
        return min_;
    }
    //-----------------------------------------------------
    const argument_type&
    max() const noexcept {
        return max_;
    }
    //-----------------------------------------------------
    const argument_type&
    bin_width() const noexcept {
        return width_;
    }


    //---------------------------------------------------------------
    void
    insert(const argument_type& x) {
        if(x >= min_ && (x < max_)) {
            ++bins_[static_cast<size_type>((x - min_) / width_)];
        }
    }
    //-----------------------------------------------------
    template<class InputIterator>
    void
    insert(InputIterator begin, InputIterator end) {
        for(; begin != end; ++begin) {
            insert(*begin);
        }
    }


    //---------------------------------------------------------------
    /// @brief lookup
    value_type
    operator () (const argument_type& x) const noexcept {
        //find x's bin and return current count
        return range_includes(x)
            ? bins_[static_cast<size_type>((x - min_) / width_)]
            : value_type(0);
    }
    //-----------------------------------------------------
    bool
    range_includes(const argument_type& x) const noexcept {
        return (x >= min_ && (x < max_));
    }
    //-----------------------------------------------------
    iterator
    find(const argument_type& x) const noexcept {
        return range_includes(x)
            ? begin() + static_cast<size_type>((x - min_) / width_)
            : end();
    }


    //-----------------------------------------------------
    const value_type&
    operator [] (size_type idx) const noexcept {
        return bins_[idx];
    }
    value_type&
    operator [] (size_type idx) noexcept {
        return bins_[idx];
    }

    //-----------------------------------------------------
    size_type
    size() const noexcept {
        return bins_.size();
    }
    //-----------------------------------------------------
    bool
    empty() const noexcept {
        return bins_.empty();
    }


    //-----------------------------------------------------
    value_type
    total() const {
        return std::accumulate(begin(), end(), argument_type(0));
    }


    //---------------------------------------------------------------
    iterator
    begin() noexcept {
        using std::begin;
        return begin(bins_);
    }
    //-----------------------------------------------------
    const_iterator
    begin() const noexcept {
        using std::begin;
        return begin(bins_);
    }
    //-----------------------------------------------------
    const_iterator
    cbegin() const noexcept {
        using std::begin;
        return begin(bins_);
    }

    //-----------------------------------------------------
    iterator
    end() noexcept {
        using std::end;
        return end(bins_);
    }
    //-----------------------------------------------------
    const_iterator
    end() const noexcept {
        using std::end;
        return end(bins_);
    }
    //-----------------------------------------------------
    const_iterator
    cend() const noexcept {
        using std::end;
        return end(bins_);
    }


private:
    //---------------------------------------------------------------
    static constexpr size_type
    required_size(const argument_type& min, const argument_type& max,
                  const argument_type& width) noexcept
    {
        return (width > 0)
            ? static_cast<size_type>(0.5 + (max - min) / width)
            : 0;
    }


    //---------------------------------------------------------------
    argument_type min_;
    argument_type max_;
    argument_type width_;
    Bins bins_;
};






/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class ArgumentT, class Bins>
inline decltype(auto)
begin(const uniform_histogram<ArgumentT,Bins>& h)
{
    return h.begin();
}
//---------------------------------------------------------
template<class ArgumentT, class Bins>
inline decltype(auto)
cbegin(const uniform_histogram<ArgumentT,Bins>& h)
{
    return h.begin();
}

//---------------------------------------------------------
template<class ArgumentT, class Bins>
inline decltype(auto)
end(const uniform_histogram<ArgumentT,Bins>& h)
{
    return h.end();
}
//---------------------------------------------------------
template<class ArgumentT, class Bins>
inline decltype(auto)
cend(const uniform_histogram<ArgumentT,Bins>& h)
{
    return h.end();
}



//-------------------------------------------------------------------
template<class ArgumentT, class Bins>
inline decltype(auto)
min(const uniform_histogram<ArgumentT,Bins>& h)
{
    return h.min();
}

//---------------------------------------------------------
template<class ArgumentT, class Bins>
inline decltype(auto)
max(const uniform_histogram<ArgumentT,Bins>& h)
{
    return h.max();
}



//---------------------------------------------------------------
template<class Ostream, class ArgumentT, class Bins>
inline Ostream&
operator << (Ostream& os, const uniform_histogram<ArgumentT,Bins>& h)
{
    using size_t_ = typename uniform_histogram<ArgumentT,Bins>::size_type;

    if(h.size() < 1) return os;

    os << h[0];
    for(size_t_ i = 1; i < h.size(); ++i) {
        os << ' ' << h[i];
    }
    return os;
}

//---------------------------------------------------------------
template<class Ostream, class ArgumentT, class Bins>
inline Ostream&
print(Ostream& os, const uniform_histogram<ArgumentT,Bins>& h)
{
    using size_t_ = typename uniform_histogram<ArgumentT,Bins>::size_type;

    if(h.size() < 1) return os;

    os << '{' << h[0];
    for(size_t_ i = 1; i < h.size(); ++i) {
        os << ',' << h[i];
    }
    return os << '}';
}



//-------------------------------------------------------------------
template<class ArgumentT, class Bins>
inline std::vector<typename std::common_type<ArgumentT,double>::type>
relative_frequencies(const uniform_histogram<ArgumentT,Bins>& h)
{
    using fp_t = typename std::common_type<ArgumentT,double>::type;
    using res_t = std::vector<fp_t>;

    auto res = res_t();
    res.assign(h.begin(), h.end());

    const auto rn = fp_t(1) / static_cast<fp_t>(h.total());
    for(auto& x :res) {
        x *= rn;
    }

    return res;
}


}  // namespace am

#endif
