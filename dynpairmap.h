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
 * @brief   stores pairwise values v = f(i,j) where i < j
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
			p_{nullptr}, mark_{nullptr}, end_{nullptr}, count_{0}, stride_{0}
		{}
		//-----------------------------------------------------
		explicit constexpr
		iter__(iterator_type start, difference_type num = 0):
			p_{start}, mark_{start + num - 1},
			end_{(num < 2) ? p_ : (start - 1 + num*(num-1)) },
			count_{num}, stride_{2}
		{}


		//---------------------------------------------------------------
		iter__&
		operator ++ () {
			++p_;
			if(p_ == mark_ && p_ != end_) {
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
		explicit
		operator bool() const {
			return (p_ && (p_!= end_));
		}

		//-----------------------------------------------------
		constexpr iter__
		end() const {
			return iter__{end_};
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
		iterator_type const end_;
		difference_type count_, stride_;
	};




	//-------------------------------------------------------------------
	//
	//-------------------------------------------------------------------
	template<class T>
	class pair_iter__
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
		pair_iter__() :
			p_{nullptr}, mark_{nullptr}, end_{nullptr}, stride_{0}
		{}
		//-----------------------------------------------------
		constexpr explicit
		pair_iter__(
			iterator_type start, difference_type num, difference_type index)
		:
			p_(start+((index<1)?1:index)), mark_(p_+index*num),
			end_(start+(index+1)*num), stride_((index<1)?1:num)
		{}

		//---------------------------------------------------------------
		pair_iter__&
		operator ++ () {
			p_ += stride_;
			if(p_ == mark_) {
				++p_;
				stride_ = 1;
			}
			return *this;
		}
		//-----------------------------------------------------
		pair_iter__
		operator ++ (int) {
			pair_iter__ old(*this);
			++*this;
			return old;
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

		//-----------------------------------------------------
		explicit
		operator bool() const {
			return (p_ && (p_!= end_));
		}

		//-----------------------------------------------------
		constexpr pair_iter__
		end() const {
			return pair_iter__{end_};
		}

		//---------------------------------------------------------------
		bool operator == (const pair_iter__& other) const {
			return (p_ == other.p_);
		}
		bool operator != (const pair_iter__& other) const {
			return (p_ != other.p_);
		}

	private:
		iterator_type p_;
		iterator_type const mark_;
		iterator_type const end_;
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
	using       pair_iterator = pair_iter__<storage_iter__>;
	using const_pair_iterator = pair_iter__<storage_citer__>;


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
	assign(size_type index, const value_type& value)
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
	clear() {
		vals_.clear();
	}


	//---------------------------------------------------------------
	// GETTERS
	//---------------------------------------------------------------
	size_type
	size() const {
		return vals_.cols();
	}
	//-----------------------------------------------------
	size_type
	num_values() const {
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
	resize(size_type size) {
		vals_.resize(size-1, size);
	}
	//-----------------------------------------------------
	void
	resize(size_type size, const value_type& value) {
		vals_.resize(size-1, size, value);
	}
	//-----------------------------------------------------
	void
	push_back() {
		vals_.cols(vals_.cols()+1);
		vals_.rows(vals_.rows()+1);
	}
	//-----------------------------------------------------
	void
	push_back(const value_type& value) {
		push_back();

		size_type n = vals_.rows();

		for(size_type i = 0; i < n; ++i) {
			vals_(i,n) = value;
		}
	}
	//-----------------------------------------------------
	void
	insert(size_type index) {
		push_back();

		for(size_type r = 0; r < index; ++r) {
			for(size_type c = vals_.rows(); c > index; --c) {
				vals_(r,c) = vals_(r,c-1);
			}
		}

		for(size_type r = vals_.rows(); r > index; --r) {
			for(size_type c = vals_.cols(); c > r; --c) {
				vals_(r,c) = vals_(r-1,c-1);
			}
		}
	}
	//-----------------------------------------------------
	void
	insert(size_type index, const value_type& value) {
		insert(index);

		for(size_type i = 0; i < index; ++i) {
			vals_(i,index) = value;
		}
		for(size_type i = index+1; i < vals_.cols(); ++i) {
			vals_(index,i) = value;
		}
	}


	//---------------------------------------------------------------
	void
	pop_back() {
		vals_.erase_row(vals_.rows()-1);
		vals_.erase_col(vals_.cols()-1);
	}
	//-----------------------------------------------------
	void
	erase(size_type index)
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

		pop_back();
	}

	//-----------------------------------------------------
	void
	swap(size_type idx1, size_type idx2) {
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
		return iterator(vals_.end());
	}
	const_iterator
	end() const {
		return const_iterator(vals_.end());
	}
	const_iterator
	cend() const {
		return const_iterator(vals_.end());
	}

	//-----------------------------------------------------
	pair_iterator
	pair_iter(size_type index) {
		return pair_iterator(vals_.begin(), vals_.cols(), index);
	}
	const_pair_iterator
	pair_iter(size_type index) const {
		return const_pair_iterator(vals_.begin(), vals_.cols(), index);
	}
	const_pair_iterator
	pair_citer(size_type index) const {
		return const_pair_iterator(vals_.begin(), vals_.cols(), index);
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
cbegin(const dynpairmap<T>& m) -> decltype(m.begin())
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
cend(const dynpairmap<T>& m) -> decltype(m.end())
{
	return m.cend();
}





}  // namespace am


#endif
