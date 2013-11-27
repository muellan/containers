/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_CONTAINERS_PAIRMAP_H_
#define AM_CONTAINERS_PAIRMAP_H_


#include <array>
#include <iterator>

#include "matrix.h"


namespace am {


/*****************************************************************************
 *
 * @brief   stores pairwise values v = f(i,j) where i < j
 * @details values are stored in a matrix
 *          (only upper triangle matrix is used)
 *
 *
 *****************************************************************************/
template<class ValueType, std::size_t numElems>
class pairmap
{
	static_assert(numElems > 1, "pairmap<T,n> : n must be at least 2");

	using storage__ = matrix<ValueType,numElems-1,numElems>;
	using storage_iter__ = typename storage__::iterator;
	using storage_citer__ = typename storage__::const_iterator;


	//-------------------------------------------------------------------
	//
	//-------------------------------------------------------------------
	template<class T>
	class iter__ :
		public std::iterator<std::forward_iterator_tag, T>
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
			p_{nullptr}, mark_{nullptr}, end_{nullptr}, stride_(0)
		{}
		//-----------------------------------------------------
		explicit constexpr
		iter__(iterator_type start):
			p_{start},
			mark_{start-1+numElems},
			end_{(numElems<2) ? p_ : (start-1+numElems*(numElems-1)) },
			stride_{2}
		{}


		//---------------------------------------------------------------
		iter__&
		operator ++ () {
			++p_;
			if(p_ == mark_ && p_ != end_) {
				p_ += stride_;
				++stride_;
				mark_ += numElems;
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
		difference_type stride_;
	};




	//-------------------------------------------------------------------
	//
	//-------------------------------------------------------------------
	template<class T>
	class local_iter__ :
		public std::iterator<std::forward_iterator_tag, T>
	{
		using base_t = std::iterator<std::forward_iterator_tag, T>;
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
			p_{nullptr}, mark_{nullptr}, end_{nullptr}, stride_(0)
		{}
		//-----------------------------------------------------
		explicit constexpr
		local_iter__(iterator_type start):
			p_(start), mark_(nullptr), end_(start), stride_(0)
		{}
		//-----------------------------------------------------
		explicit constexpr
		local_iter__(iterator_type start, difference_type index):
			p_(start+((index<1)?1:index)), mark_(p_+index*numElems),
			end_(start+(index+1)*numElems), stride_((index<1)?1:numElems)
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
		constexpr local_iter__
		end() const {
			return local_iter__{end_};
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
	using       local_iterator = local_iter__<storage_iter__>;
	using const_local_iterator = local_iter__<storage_citer__>;


	//---------------------------------------------------------------
	class memento {
		friend class pairmap;
	public:
		//-----------------------------------------------------
		memento() = default;

		//-----------------------------------------------------
		memento(const pairmap& source, size_type index):
			vals_{}
		{
			for(size_type i = 0; i < index; ++i) {
				vals_[i] = source.vals_(i,index);
			}
			for(size_type i = index+1; i < numElems; ++i) {
				vals_[i-1] = source.vals_(index,i);
			}
		}

		//-----------------------------------------------------
		void
		backup(const pairmap& source, size_type index)
		{
			for(size_type i = 0; i < index; ++i) {
				vals_[i] = source.vals_(i,index);
			}
			for(size_type i = index+1; i < numElems; ++i) {
				vals_[i-1] = source.vals_(index,i);
			}
		}

	private:
		std::array<value_type,numElems-1> vals_;
	};


	//---------------------------------------------------------------
	// CONSTRUCTION
	//---------------------------------------------------------------
	pairmap() = default;
	//-----------------------------------------------------
	pairmap(const pairmap&) = default;
	//-----------------------------------------------------
	pairmap(pairmap&&) = default;


	//---------------------------------------------------------------
	// SETTERS
	//---------------------------------------------------------------
	pairmap& operator = (const pairmap&) = default;
	//-----------------------------------------------------
	pairmap&
	operator = (pairmap&&) = default;
	//-----------------------------------------------------
	pairmap&
	operator = (const value_type& value)
	{
		for(size_type i = 0; i < (numElems-1); ++i) {
			for(size_type j = i+1; j < numElems; ++j) {
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
		for(size_type i = index+1; i < numElems; ++i) {
			vals_(index,i) = value;
		}
	}
	//-----------------------------------------------------
	void
	assign_index(size_type index, const memento& mem) {
		for(size_type i = 0; i < index; ++i) {
			vals_(i,index) = mem.vals_[i];
		}
		for(size_type i = index+1; i < numElems; ++i) {
			vals_(index,i) = mem.vals_[i-1];
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
	static constexpr size_type
	index_count() noexcept {
		return numElems;
	}
	//-----------------------------------------------------
	static constexpr size_type
	size() noexcept {
		return (numElems * (numElems-1)) / size_type(2);
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

		for(size_type i = idx2+1; i < (numElems-1); ++i) {
			swap(vals_(idx1,i), vals_(idx2,i));
		}
	}


	//---------------------------------------------------------------
	// ACCESSSORS
	//---------------------------------------------------------------
	iterator
	begin() {
		return iterator(vals_.begin() + 1);
	}
	const_iterator
	begin() const {
		return const_iterator(vals_.begin() + 1);
	}
	const_iterator
	cbegin() const {
		return const_iterator(vals_.begin() + 1);
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
	local_iterator
	begin(size_type index) {
		return local_iterator(vals_.begin(), index);
	}
	const_local_iterator
	begin(size_type index) const {
		return const_local_iterator(vals_.begin(), index);
	}
	const_local_iterator
	cbegin(size_type index) const {
		return const_local_iterator(vals_.begin(), index);
	}

	//-----------------------------------------------------
	local_iterator
	end(size_type index) {
		return local_iterator(vals_.begin() + ((index+1)*numElems));
	}
	const_local_iterator
	end(size_type index) const {
		return const_local_iterator(vals_.begin() + ((index+1)*numElems));
	}
	const_local_iterator
	cend(size_type index) const {
		return const_local_iterator(vals_.begin() + ((index+1)*numElems));
	}

	//---------------------------------------------------------------
//	inline friend std::ostream&
//	operator << (std::ostream& os, const pairmap& o) {
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
template<class T, std::size_t n>
inline auto
begin(pairmap<T,n>& m) -> decltype(m.begin())
{
	return m.begin();
}

//---------------------------------------------------------
template<class T, std::size_t n>
inline auto
begin(const pairmap<T,n>& m) -> decltype(m.begin())
{
	return m.begin();
}

//---------------------------------------------------------
template<class T, std::size_t n>
inline auto
cbegin(const pairmap<T,n>& m) -> decltype(m.begin())
{
	return m.cbegin();
}



//-------------------------------------------------------------------
template<class T, std::size_t n>
inline auto
end(pairmap<T,n>& m) -> decltype(m.end())
{
	return m.end();
}

//---------------------------------------------------------
template<class T, std::size_t n>
inline auto
end(const pairmap<T,n>& m) -> decltype(m.end())
{
	return m.end();
}

//---------------------------------------------------------
template<class T, std::size_t n>
inline auto
cend(const pairmap<T,n>& m) -> decltype(m.end())
{
	return m.cend();
}


}  // namespace am


#endif
