/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
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
template<class ValueType, std::size_t maxIndex>
class pairmap
{
	static_assert(maxIndex > 0, "pairmap<T,maxIdx> : maxIdx must be at least 1");

	using storage__ = matrix<ValueType,maxIndex,maxIndex+1>;
	using storage_iter__ = typename storage__::iterator;
	using storage_citer__ = typename storage__::const_iterator;


	//-------------------------------------------------------------------
	//
	//-------------------------------------------------------------------
	template<class Iter>
	class iter__
	{
		using traits__ = std::iterator_traits<Iter>;
	public:
		//---------------------------------------------------------------
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename traits__::value_type;
		using pointer = typename traits__::pointer;
		using reference = typename traits__::reference;
		using difference_type = typename traits__::difference_type;


		//---------------------------------------------------------------
		constexpr
		iter__():
			p_(), mark_(), stride_()
		{}
		//-----------------------------------------------------
		explicit constexpr
		iter__(Iter start):
			p_(start), mark_(start+maxIndex), stride_(2)
		{}
		//-----------------------------------------------------
		explicit constexpr
		iter__(Iter start, difference_type offset):
			p_(start+offset), mark_(p_ + maxIndex+1 - (offset % (maxIndex+1))),
			stride_(2 + (offset / (maxIndex+1)))
		{}


		//---------------------------------------------------------------
		iter__&
		operator ++ () {
			++p_;
			if(p_ == mark_) {
				p_ += stride_;
				++stride_;
				mark_ += maxIndex+1;
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
		operator * () const
			-> decltype(*std::declval<Iter>())
		{
			return *p_;
		}
		//-----------------------------------------------------
		auto
		operator -> () const
			-> decltype(std::addressof(*std::declval<Iter>()))
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
		Iter p_;
		Iter mark_;
		difference_type stride_;
	};



	//-------------------------------------------------------------------
	template<class Iter>
	class index_iter__
	{
		using traits__ = std::iterator_traits<Iter>;
	public:
		//---------------------------------------------------------------
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename traits__::value_type;
		using pointer = typename traits__::pointer;
		using reference = typename traits__::reference;
		using difference_type = typename traits__::difference_type;


		//---------------------------------------------------------------
		constexpr
		index_iter__() :
			p_(), mark_(), stride_()
		{}
		//-----------------------------------------------------
		explicit constexpr
		index_iter__(Iter start):
			p_(start), mark_(), stride_()
		{}
		//-----------------------------------------------------
		explicit constexpr
		index_iter__(Iter start, difference_type index):
			p_(start+((index<1)?1:index)), mark_(p_+index*(maxIndex+1)),
			stride_((index<1) ? 1 : (maxIndex+1))
		{}

		//---------------------------------------------------------------
		index_iter__&
		operator ++ () {
			p_ += stride_;
			if(p_ == mark_) {
				++p_;
				stride_ = 1;
			}
			return *this;
		}
		//-----------------------------------------------------
		index_iter__
		operator ++ (int) {
			index_iter__ old(*this);
			++*this;
			return old;
		}

		//---------------------------------------------------------------
		auto
		operator * () const
			-> decltype(*std::declval<Iter>())
		{
			return *p_;
		}
		//-----------------------------------------------------
		auto
		operator -> () const
			-> decltype(std::addressof(*std::declval<Iter>()))
		{
			return std::addressof(*p_);
		}

		//---------------------------------------------------------------
		bool operator == (const index_iter__& other) const {
			return (p_ == other.p_);
		}
		bool operator != (const index_iter__& other) const {
			return (p_ != other.p_);
		}

	private:
		Iter p_;
		Iter mark_;
		difference_type stride_;
	};



	//---------------------------------------------------------------
	template<class Iter>
	struct section__
	{
		//-----------------------------------------------------
		struct iterator
		{
			using traits__ = std::iterator_traits<Iter>;
		public:
			//-------------------------------------------
			using iterator_category = std::forward_iterator_tag;
			using value_type = typename traits__::value_type;
			using pointer = typename traits__::pointer;
			using reference = typename traits__::reference;
			using difference_type = typename traits__::difference_type;

			//-------------------------------------------
			explicit constexpr
			iterator(Iter p):
				p_(p), mark_(), mark2_(), stride_(), dnf_()
			{}
			//-------------------------------------------
			explicit constexpr
			iterator(Iter p, difference_type fidx, difference_type lidx):
				p_(p),
				mark_(p	+ ((fidx > 0) ? (lidx - fidx) : (maxIndex - 1))),
				mark2_(mark_ + ((fidx > 0) ? ((fidx-1) * (maxIndex+1)) : 0)),
				stride_((fidx > 0) ? (1 + maxIndex + fidx - lidx) : 2),
				dnf_(maxIndex - fidx - ((fidx > 0) ? 1 : 2))
			{}

			//-------------------------------------------
			iterator&
			operator ++ () {
				if(p_ == mark_) {
					if(p_ == mark2_) {
						p_ += stride_ + 1;
						mark2_ = p_ + dnf_;
						stride_ = maxIndex - dnf_ + 2;
					} else {
						p_ += stride_;
						mark_ += maxIndex + 1;
					}
				} else if(p_ == mark2_) {
					p_ += stride_;
					mark2_ += maxIndex + 1;
					++stride_;
				} else {
					++p_;
				}
				return *this;
			}
			//-------------------------------------------
			iterator
			operator ++ (int) {
				iterator old(*this);
				++*this;
				return old;
			}

			//-------------------------------------------
			auto
			operator * () const
				-> decltype(*std::declval<Iter>())
			{
				return *p_;
			}
			//-------------------------------------------
			auto
			operator -> () const
				-> decltype(std::addressof(*std::declval<Iter>()))
			{
				return std::addressof(*p_);
			}

			//-------------------------------------------
			bool operator == (const iterator& other) const {
				return (p_ == other.p_);
			}
			bool operator != (const iterator& other) const {
				return (p_ != other.p_);
			}

		private:
			Iter p_;
			Iter mark_;
			Iter mark2_;
			difference_type stride_;
			difference_type dnf_;
		};


		//-----------------------------------------------------
		using difference_type = typename iterator::difference_type;

		//-----------------------------------------------------
		constexpr
		section__() = default;
		//-----------------------------------------------------
		explicit constexpr
		section__(Iter beg, difference_type fidx, difference_type lidx):
			fst_{beg + ((fidx > 1) ? (fidx-1) : 0)},
			lst_{beg + ((lidx+1)*(maxIndex+1) - 1) +
				((lidx < difference_type(maxIndex)) ? (lidx + 2) : 0)},
			fidx_{fidx}, lidx_{lidx}
		{}

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
		Iter fst_, lst_;
		difference_type fidx_, lidx_;
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
	using       index_iterator = index_iter__<storage_iter__>;
	using const_index_iterator = index_iter__<storage_citer__>;
	//-----------------------------------------------------
	using       section = section__<storage_iter__>;
	using const_section = section__<storage_citer__>;


	//---------------------------------------------------------------
	class memento {
	public:
		//-----------------------------------------------------
		memento() = default;

		//-----------------------------------------------------
		memento(const pairmap& source, size_type index):
			vals_{}
		{
			backup(source,index);
		}

		//-----------------------------------------------------
		void
		backup(const pairmap& source, size_type index)
		{
			for(size_type i = 0; i < index; ++i) {
				vals_[i] = source.vals_(i,index);
			}
			for(size_type i = index+1; i <= maxIndex; ++i) {
				vals_[i-1] = source.vals_(index,i);
			}
		}

		//-----------------------------------------------------
		void
		restore(pairmap& target, size_type index) const {
			for(size_type i = 0; i < index; ++i) {
				target.vals_(i,index) = vals_[i];
			}
			for(size_type i = index+1; i < vals_.cols(); ++i) {
				target.vals_(index,i) = vals_[i-1];
			}
		}

	private:
		std::array<value_type,maxIndex> vals_;
	};
	friend class memento;


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
		for(size_type i = 0; i < maxIndex; ++i) {
			for(size_type j = i+1; j <= maxIndex; ++j) {
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
		for(size_type i = index+1; i <= maxIndex; ++i) {
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
	static constexpr size_type
	min_index() noexcept {
		return 0;
	}
	//-----------------------------------------------------
	static constexpr size_type
	max_index() noexcept {
		return maxIndex;
	}

	//-----------------------------------------------------
	static constexpr size_type
	size() noexcept {
		return (maxIndex * (maxIndex+1)) / size_type(2);
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
	static constexpr bool
	contains(size_type idx1, size_type idx2) noexcept {
		return (idx1 <= maxIndex) && (idx2 <= maxIndex);
	}

	//-----------------------------------------------------
	iterator
	find(size_type idx1, size_type idx2) {

		if(!contains(idx1,idx2)) return end();

		return iterator(vals_.begin(),
			(idx1 < idx2) ?
				idx1 * vals_.cols() + idx2 : idx2 * vals_.cols() + idx1);
	}
	//-----------------------------------------------------
	const_iterator
	find(size_type idx1, size_type idx2) const {

		if(!contains(idx1,idx2)) return end();

		return const_iterator(vals_.begin(),
			(idx1 < idx2) ?
				idx1 * vals_.cols() + idx2 : idx2 * vals_.cols() + idx1);
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

		for(size_type i = idx2+1; i < maxIndex; ++i) {
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
		return iterator(vals_.end() + maxIndex + 1);
	}
	const_iterator
	end() const {
		return const_iterator(vals_.end() + maxIndex + 1);
	}
	const_iterator
	cend() const {
		return const_iterator(vals_.end() + maxIndex + 1);
	}

	//-----------------------------------------------------
	index_iterator
	begin(size_type index) {
		return index_iterator(vals_.begin(), index);
	}
	const_index_iterator
	begin(size_type index) const {
		return const_index_iterator(vals_.begin(), index);
	}
	const_index_iterator
	cbegin(size_type index) const {
		return const_index_iterator(vals_.begin(), index);
	}

	//-----------------------------------------------------
	index_iterator
	end(size_type index) {
		return index_iterator(vals_.begin() + ((index+1)*(maxIndex+1)));
	}
	const_index_iterator
	end(size_type index) const {
		return const_index_iterator(vals_.begin() + ((index+1)*(maxIndex+1)));
	}
	const_index_iterator
	cend(size_type index) const {
		return const_index_iterator(vals_.begin() + ((index+1)*(maxIndex+1)));
	}


	//---------------------------------------------------------------
	// SECTIONS
	//---------------------------------------------------------------
	section
	subrange(size_type firstIncl, size_type lastIncl) {
		return section(vals_.begin() + 1, firstIncl, lastIncl);
	}
	const_section
	subrange(size_type firstIncl, size_type lastIncl) const {
		return const_section(vals_.begin() + 1, firstIncl, lastIncl);
	}
	const_section
	csubrange(size_type firstIncl, size_type lastIncl) const {
		return const_section(vals_.begin() + 1, firstIncl, lastIncl);
	}


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
