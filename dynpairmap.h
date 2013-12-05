/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_CONTAINERS_DYNPAIRMAP_H_
#define AM_CONTAINERS_DYNPAIRMAP_H_


#include <vector>
#include <iterator>

#include "dynmatrix.h"


namespace am {


/*****************************************************************************
 *
 * @brief   stores pairwise values v = f(i,j)
 * @details values are stored in a matrix
 *          (only upper triangle matrix is used)
 *
 *
 *****************************************************************************/
template<class ValueType>
class dynpairmap
{
	using storage__ = dynmatrix<ValueType>;
	using storage_iter__ = typename storage__::iterator;
	using storage_citer__ = typename storage__::const_iterator;


	//-------------------------------------------------------------------
	//
	//-------------------------------------------------------------------
	template<class T>
	class iter__
	{
	public:
		//---------------------------------------------------------------
		using iterator_category = std::forward_iterator_tag;
		using iterator_type = T;
		using value_type = typename std::iterator_traits<T>::value_type;
		using pointer = typename std::iterator_traits<T>::pointer;
		using reference = typename std::iterator_traits<T>::reference;
		using difference_type = typename std::iterator_traits<T>::difference_type;


		//---------------------------------------------------------------
		constexpr
		iter__():
			p_{nullptr}, mark_{nullptr}, count_{0}, stride_{0}
		{}
		//-----------------------------------------------------
		explicit constexpr
		iter__(iterator_type start, difference_type num = 0):
			p_{start}, mark_{start + num - 1}, count_{num}, stride_{2}
		{}


		//---------------------------------------------------------------
		iter__&
		operator ++ () {
			++p_;
			if(p_ == mark_) {
				p_ += stride_;
				++stride_;
				mark_ += count_;
			}
			return *this;
		}
		//-----------------------------------------------------
		iter__
		operator ++ (int) {
			iter__ old(*this);
			++*this;
			return old;
		}
		//-----------------------------------------------------
		iter__&
		operator += (difference_type diff) {
			for(difference_type i = 0; i < diff; ++i) {
				operator++();
			}
			return *this;
		}
		//-----------------------------------------------------
		iter__
		operator + (difference_type diff) const {
			auto cp(*this);
			cp += diff;
			return cp;
		}

		//---------------------------------------------------------------
		auto
		operator * () -> decltype(*std::declval<T>())
		{
			return *p_;
		}
		//-----------------------------------------------------
		auto
		operator -> () -> decltype(std::addressof(*std::declval<T>()))
		{
			return std::addressof(*p_);
		}

		//---------------------------------------------------------------
		bool operator == (const iter__& other) const {
			return (p_ == other.p_);
		}
		bool operator != (const iter__& other) const {
			return (p_ != other.p_);
		}

	private:
		iterator_type p_;
		iterator_type mark_;
		difference_type count_, stride_;
	};




	//-------------------------------------------------------------------
	//
	//-------------------------------------------------------------------
	template<class T>
	class local_iter__
	{
	public:
		//---------------------------------------------------------------
		using iterator_category = std::forward_iterator_tag;
		using iterator_type = T;
		using value_type = typename std::iterator_traits<T>::value_type;
		using pointer = typename std::iterator_traits<T>::pointer;
		using reference = typename std::iterator_traits<T>::reference;
		using difference_type = typename std::iterator_traits<T>::difference_type;


		//---------------------------------------------------------------
		constexpr
		local_iter__() :
			p_{nullptr}, mark_{nullptr}, stride_(0)
		{}
		//-----------------------------------------------------
		constexpr explicit
		local_iter__(iterator_type start) :
			p_(start), mark_(nullptr), stride_(0)
		{}
		//-----------------------------------------------------
		constexpr explicit
		local_iter__(
			iterator_type start, difference_type num, difference_type index)
		:
			p_(start+((index<1)?1:index)), mark_(p_+index*num),
			stride_((index<1)?1:num)
		{}

		//---------------------------------------------------------------
		local_iter__&
		operator ++ () {
			p_ += stride_;
			if(p_ == mark_) {
				++p_;
				stride_ = 1;
			}
			return *this;
		}
		//-----------------------------------------------------
		local_iter__
		operator ++ (int) {
			local_iter__ old(*this);
			++*this;
			return old;
		}
		//-----------------------------------------------------
		local_iter__&
		operator += (difference_type diff) {
			for(difference_type i = 0; i < diff; ++i) {
				operator++();
			}
			return *this;
		}
		//-----------------------------------------------------
		local_iter__
		operator + (difference_type diff) const {
			auto cp(*this);
			cp += diff;
			return cp;
		}

		//---------------------------------------------------------------
		auto
		operator * () -> decltype(*std::declval<T>())
		{
			return *p_;
		}
		//-----------------------------------------------------
		auto
		operator -> () -> decltype(std::addressof(*std::declval<T>()))
		{
			return std::addressof(*p_);
		}

		//---------------------------------------------------------------
		bool operator == (const local_iter__& other) const {
			return (p_ == other.p_);
		}
		bool operator != (const local_iter__& other) const {
			return (p_ != other.p_);
		}

	private:
		iterator_type p_;
		iterator_type mark_;
		difference_type stride_;
	};


public:

	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using value_type = ValueType;
	using size_type = typename storage__::size_type;
	//-----------------------------------------------------
	using       iterator = iter__<storage_iter__>;
	using const_iterator = iter__<storage_citer__>;
	//-----------------------------------------------------
	using       local_iterator = local_iter__<storage_iter__>;
	using const_local_iterator = local_iter__<storage_citer__>;


	//---------------------------------------------------------------
	class memento {
	public:
		//-----------------------------------------------------
		memento() = default;

		//-----------------------------------------------------
		memento(const dynpairmap& source, size_type index):
			vals_(source.vals_.rows())
		{
			for(size_type i = 0; i < index; ++i) {
				vals_[i] = source.vals_(i,index);
			}
			for(size_type i = index+1; i < source.vals_.cols(); ++i) {
				vals_[i-1] = source.vals_(index,i);
			}
		}

		//-----------------------------------------------------
		void
		backup(const dynpairmap& source, size_type index) {
			if(vals_.size() < source.vals_.rows()) {
				vals_.resize(source.vals_.rows());
			}
			for(size_type i = 0; i < index; ++i) {
				vals_[i] = source.vals_(i,index);
			}
			for(size_type i = index+1; i < source.vals_.cols(); ++i) {
				vals_[i-1] = source.vals_(index,i);
			}
		}

		//-----------------------------------------------------
		void
		restore(dynpairmap& target, size_type index) const {
			for(size_type i = 0; i < index; ++i) {
				target.vals_(i,index) = vals_[i];
			}
			for(size_type i = index+1; i < vals_.cols(); ++i) {
				target.vals_(index,i) = vals_[i-1];
			}
		}

	private:
		std::vector<value_type> vals_;
	};
	friend class memento;


	//---------------------------------------------------------------
	// CONSTRUCTION
	//---------------------------------------------------------------
	dynpairmap():
		vals_{}
	{
		vals_.resize(0,1);
	}
	//-----------------------------------------------------
	dynpairmap(const dynpairmap&) = default;
	//-----------------------------------------------------
	dynpairmap(dynpairmap&& source):
		vals_{std::move(source.vals_)}
	{}


	//---------------------------------------------------------------
	// SETTERS
	//---------------------------------------------------------------
	dynpairmap& operator = (const dynpairmap&) = default;
	//-----------------------------------------------------
	dynpairmap&
	operator = (dynpairmap&& source) {
		vals_ = std::move(source.vals_);
		return *this;
	}
	//-----------------------------------------------------
	dynpairmap&
	operator = (const value_type& value)
	{
		for(size_type i = 0; i < vals_.rows(); ++i) {
			for(size_type j = i+1; j < vals_.cols(); ++j) {
				vals_(i,j) = value;
			}
		}
		return *this;
	}

	//-----------------------------------------------------
	void
	assign_index(size_type index, const value_type& value)
	{
		for(size_type i = 0; i < index; ++i){
			vals_(i,index) = value;
		}
		for(size_type i = index+1; i < vals_.cols(); ++i) {
			vals_(index,i) = value;
		}
	}
	//-----------------------------------------------------
	void
	assign_index(size_type index, const memento& mem) {
		mem.restore(*this, index);
	}

	//-----------------------------------------------------
	void
	clear() {
		vals_.clear();
	}


	//---------------------------------------------------------------
	// GETTERS
	//---------------------------------------------------------------
	size_type
	index_count() const {
		return vals_.cols();
	}
	//-----------------------------------------------------
	static constexpr size_type
	min_index() noexcept {
		return 0;
	}
	//-----------------------------------------------------
	size_type
	max_index() const {
		return vals_.rows();
	}

	//-----------------------------------------------------
	size_type
	size() const {
		return (vals_.cols() * vals_.rows()) / size_type(2);
	}

	//-----------------------------------------------------
	value_type&
	operator () (size_type idx1, size_type idx2) {
		return (idx1 < idx2) ? vals_(idx1,idx2) : vals_(idx2,idx1);
	}
	//-----------------------------------------------------
	const value_type&
	operator () (size_type idx1, size_type idx2) const {
		return (idx1 < idx2) ? vals_(idx1,idx2) : vals_(idx2,idx1);
	}


	//---------------------------------------------------------------
	// CHANGE n
	//---------------------------------------------------------------
	void
	reserve_indices(size_type size) {
		vals_.reserve(size-1, size);
	}
	//-----------------------------------------------------
	void
	resize_indices(size_type size) {
		vals_.resize(size-1, size);
	}
	//-----------------------------------------------------
	void
	resize_indices(size_type size, const value_type& value) {
		vals_.resize(size-1, size, value);
	}

	//-----------------------------------------------------
	void
	increase_indices_from(size_type index, size_type n = 1) {
		if(n < 1) return;
		vals_.insert_rows(index, n);
		vals_.insert_cols(index, n);
	}
	//-----------------------------------------------------
	void
	increase_indices_from(
		size_type index, size_type n, const value_type& value)
	{
		if(n < 1) return;

		increase_indices_from(index,n);

		for(size_type j = index; j < index+n; ++j) {
			for(size_type i = 0; i < j; ++i) {
				vals_(i,j) = value;
			}
			for(size_type i = j+1; i < vals_.cols(); ++i) {
				vals_(j,i) = value;
			}
		}
	}


	//---------------------------------------------------------------
	void
	erase_index_decrease_above(size_type index)
	{
		for(size_type r = 0; r < index; ++r) {
			for(size_type c = index; c < vals_.rows(); ++c) {
				vals_(r,c) = vals_(r,c+1);
			}
		}

		for(size_type r = index+1; r < vals_.rows(); ++r) {
			for(size_type c = r+1; c < vals_.cols(); ++c) {
				vals_(r-1,c-1) = vals_(r,c);
			}
		}

		vals_.erase_row(vals_.rows()-1);
		vals_.erase_col(vals_.cols()-1);
	}
	//-----------------------------------------------------
	void
	erase_range_decrease_above(size_type firstIndex, size_type lastIndex)
	{
		using std::min;

		if(firstIndex >= lastIndex) return;

		vals_.erase_rows(
			min(firstIndex,vals_.rows()-1),
			min(lastIndex,vals_.rows()-1));

		vals_.erase_cols(
			min(firstIndex,vals_.cols()-1),
			min(lastIndex,vals_.cols()-1));
	}

	//-----------------------------------------------------
	void
	swap_indices(size_type idx1, size_type idx2) {
		using std::swap;

		if(idx1 == idx2) return;

		if(idx1 > idx2) swap(idx1,idx2);

		for(size_type i = 0; i < idx1; ++i) {
			swap(vals_(i,idx1), vals_(i,idx2));
		}

		for(size_type i = idx1+1; i < idx2; ++i) {
			swap(vals_(idx1,i), vals_(i,idx2));
		}

		for(size_type i = idx2+1, n = vals_.rows(); i < n; ++i) {
			swap(vals_(idx1,i), vals_(idx2,i));
		}
	}


	//---------------------------------------------------------------
	memento
	get_memento(size_type index) const {
		return memento{*this,index};
	}


	//---------------------------------------------------------------
	// ACCESSSORS
	//---------------------------------------------------------------
	iterator
	begin() {
		return iterator(vals_.begin()+1, vals_.cols());
	}
	const_iterator
	begin() const {
		return const_iterator(vals_.begin()+1, vals_.cols());
	}
	const_iterator
	cbegin() const {
		return const_iterator(vals_.begin()+1, vals_.cols());
	}

	//-----------------------------------------------------
	iterator
	end() {
		return iterator(vals_.end() + vals_.cols());
	}
	const_iterator
	end() const {
		return const_iterator(vals_.end() + vals_.cols());
	}
	const_iterator
	cend() const {
		return const_iterator(vals_.end() + vals_.cols());
	}

	//-----------------------------------------------------
	local_iterator
	begin(size_type index) {
		return local_iterator(vals_.begin(), vals_.cols(), index);
	}
	const_local_iterator
	begin(size_type index) const {
		return const_local_iterator(vals_.begin(), vals_.cols(), index);
	}
	const_local_iterator
	cbegin(size_type index) const {
		return const_local_iterator(vals_.begin(), vals_.cols(), index);
	}

	//-----------------------------------------------------
	local_iterator
	end(size_type index) {
		return local_iterator(vals_.begin() + ((index+1) * vals_.cols()));
	}
	const_local_iterator
	end(size_type index) const {
		return const_local_iterator(vals_.begin() + ((index+1) * vals_.cols()));
	}
	const_local_iterator
	cend(size_type index) const {
		return const_local_iterator(vals_.begin() + ((index+1) * vals_.cols()));
	}


	//---------------------------------------------------------------
//	inline friend std::ostream&
//	operator << (std::ostream& os, const dynpairmap& o) {
//		return os << o.vals_;
//	}


private:

	//---------------------------------------------------------------
	storage__ vals_;
};








/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline auto
begin(dynpairmap<T>& m) -> decltype(m.begin())
{
	return m.begin();
}

//---------------------------------------------------------
template<class T>
inline auto
begin(const dynpairmap<T>& m) -> decltype(m.begin())
{
	return m.begin();
}

//---------------------------------------------------------
template<class T>
inline auto
cbegin(const dynpairmap<T>& m) -> decltype(m.cbegin())
{
	return m.cbegin();
}



//-------------------------------------------------------------------
template<class T>
inline auto
end(dynpairmap<T>& m) -> decltype(m.end())
{
	return m.end();
}

//---------------------------------------------------------
template<class T>
inline auto
end(const dynpairmap<T>& m) -> decltype(m.end())
{
	return m.end();
}

//---------------------------------------------------------
template<class T>
inline auto
cend(const dynpairmap<T>& m) -> decltype(m.cend())
{
	return m.cend();
}





}  // namespace am


#endif
