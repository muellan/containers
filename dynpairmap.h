/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
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
	using index__ = typename storage__::size_type;


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
			p_(), mark_(), count_(), stride_()
		{}
		//-----------------------------------------------------
		explicit constexpr
		iter__(Iter start, difference_type num = 0):
			p_(start), mark_(start + num - 1), count_(num), stride_(2)
		{}
		//-----------------------------------------------------
		explicit constexpr
		iter__(Iter start, difference_type num, difference_type offset):
			p_(start+offset), mark_(p_ + num - (offset % num)),
			count_(num), stride_(2 + (offset / num))
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
		difference_type count_, stride_;
	};




	//-------------------------------------------------------------------
	//
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
		constexpr explicit
		index_iter__(Iter start) :
			p_(start), mark_(), stride_()
		{}
		//-----------------------------------------------------
		constexpr explicit
		index_iter__(
			Iter start, difference_type num, difference_type index)
		:
			p_(start+((index<1)?1:index)), mark_(p_+index*num),
			stride_((index<1)?1:num)
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
			using size_type = typename storage__::size_type;
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
				p_(p), mark_(), mark2_(), stride_(), n_(), dnf_()
			{}
			//-------------------------------------------
			explicit constexpr
			iterator(Iter p,
				difference_type fidx, difference_type lidx,
				size_type n)
			:
				p_(p),
				mark_(p	+ ((fidx > 0) ? (lidx - fidx) : (n - 2))),
				mark2_(mark_ + ((fidx > 0) ? ((fidx-1) * (n)) : 0)),
				stride_((fidx > 0) ? (n + fidx - lidx) : 2),
				n_(n),
				dnf_(n - fidx - ((fidx > 0) ? 2 : 3))
			{}

			//-------------------------------------------
			iterator&
			operator ++ () {
				if(p_ == mark_) {
					if(p_ == mark2_) {
						p_ += stride_ + 1;
						mark2_ = p_ + dnf_;
						stride_ = n_ - dnf_ + 1;
					} else {
						p_ += stride_;
						mark_ += n_;
					}
				} else if(p_ == mark2_) {
					p_ += stride_;
					mark2_ += n_;
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
			size_type n_;
			difference_type dnf_;
		};


		//-----------------------------------------------------
		using difference_type = typename iterator::difference_type;
		using size_type = typename iterator::size_type;


		//-----------------------------------------------------
		constexpr
		section__() = default;
		//-----------------------------------------------------
		explicit constexpr
		section__(Iter beg,
			difference_type fidx, difference_type lidx, size_type n)
		:
			fst_{beg + ((fidx > 1) ? (fidx-1) : 0)},
			lst_{beg + ((lidx+1)*n - 1) +
				((lidx < difference_type(n-1)) ? (lidx + 2) : 0)},
			fidx_{fidx}, lidx_{lidx}, n_{n}
		{}

		//-----------------------------------------------------
		constexpr iterator
		begin() const {
			return iterator(fst_, fidx_, lidx_, n_);
		}
		//-----------------------------------------------------
		constexpr iterator
		end() const {
			return iterator(lst_);
		}


	private:
		Iter fst_, lst_;
		difference_type fidx_, lidx_;
		size_type n_;
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
		explicit
		memento(const dynpairmap& source, size_type index):
			vals_()
		{
			backup(source,index);
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
	static constexpr size_type
	min_index() noexcept {
		return 0;
	}


	//---------------------------------------------------------------
	void
	reserve_max_index(size_type index) {
		vals_.reserve(index, index+1);
	}
	//-----------------------------------------------------
	size_type
	max_index() const {
		return vals_.rows();
	}
	//-----------------------------------------------------
	void
	max_index(size_type index) {
		vals_.resize(index, index+1);
	}
	//-----------------------------------------------------
	void
	max_index(size_type index, const value_type& value) {
		vals_.resize(index, index+1, value);
	}


	//---------------------------------------------------------------
	size_type
	size() const {
		return (vals_.cols() * vals_.rows()) / size_type(2);
	}


	//---------------------------------------------------------------
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
	bool
	contains(size_type idx1, size_type idx2) const {
		return (idx1 <= max_index()) && (idx2 <= max_index());
	}

	//-----------------------------------------------------
	iterator
	find(size_type idx1, size_type idx2) {
		if(!contains(idx1,idx2)) return end();

		return iterator(vals_.begin(), vals_.cols(),
			(idx1 < idx2) ?
				idx1 * vals_.cols() + idx2 : idx2 * vals_.cols() + idx1);
	}
	//-----------------------------------------------------
	const_iterator
	find(size_type idx1, size_type idx2) const {
		if(!contains(idx1,idx2)) return end();

		return const_iterator(vals_.begin(), vals_.cols(),
			(idx1 < idx2) ?
				idx1 * vals_.cols() + idx2 : idx2 * vals_.cols() + idx1);
	}


	//---------------------------------------------------------------
	void
	increase_indices(size_type firstIndex, size_type n = 1) {
		if(n < 1) return;

		if(max_index() < 1) {
			max_index(1);
		}
		else {
			vals_.insert_rows(firstIndex, n);
			vals_.insert_cols(firstIndex, n);
		}
	}
	//-----------------------------------------------------
	void
	increase_indices(
		size_type firstIndex, size_type n, const value_type& value)
	{
		if(n < 1) return;

		increase_indices(firstIndex,n);

		for(size_type j = firstIndex; j < firstIndex+n; ++j) {
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
		if(max_index() < 2) {
			clear();
		}
		else {
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
	}
	//-----------------------------------------------------
	void
	erase_range_decrease_above(size_type firstIndex, size_type lastIndex)
	{
		using std::min;

		if(max_index() < 2) {
			 if(firstIndex < 2)	clear();
		}
		else if(firstIndex == lastIndex) {
			erase_index_decrease_above(firstIndex);
		}
		else if(firstIndex < lastIndex) {
			if(firstIndex == 0 && lastIndex == max_index()) {
				clear();
			}
			else {
				vals_.erase_rows(
					min(firstIndex,vals_.rows()-1),
					min(lastIndex,vals_.rows()-1));

				vals_.erase_cols(
					min(firstIndex,vals_.cols()-1),
					min(lastIndex,vals_.cols()-1));
			}
		}
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

		for(size_type i = idx2+1, n = vals_.cols(); i < n; ++i) {
			swap(vals_(idx1,i), vals_(idx2,i));
		}
	}


	//---------------------------------------------------------------
	memento
	get_memento(size_type index) const {
		return memento{*this,index};
	}


	//---------------------------------------------------------------
	// ITERATORS
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
	index_iterator
	begin(size_type index) {
		return index_iterator(vals_.begin(), vals_.cols(), index);
	}
	const_index_iterator
	begin(size_type index) const {
		return const_index_iterator(vals_.begin(), vals_.cols(), index);
	}
	const_index_iterator
	cbegin(size_type index) const {
		return const_index_iterator(vals_.begin(), vals_.cols(), index);
	}

	//-----------------------------------------------------
	index_iterator
	end(size_type index) {
		return index_iterator(vals_.begin() + ((index+1) * vals_.cols()));
	}
	const_index_iterator
	end(size_type index) const {
		return const_index_iterator(vals_.begin() + ((index+1) * vals_.cols()));
	}
	const_index_iterator
	cend(size_type index) const {
		return const_index_iterator(vals_.begin() + ((index+1) * vals_.cols()));
	}


	//---------------------------------------------------------------
	// SECTIONS
	//---------------------------------------------------------------
	section
	subrange(size_type firstIncl, size_type lastIncl) {
		return section(
			vals_.begin() + 1, firstIncl, lastIncl, vals_.cols());
	}
	const_section
	subrange(size_type firstIncl, size_type lastIncl) const {
		return const_section(
			vals_.begin() + 1, firstIncl, lastIncl, vals_.cols());
	}
	const_section
	csubrange(size_type firstIncl, size_type lastIncl) const {
		return const_section(
			vals_.begin() + 1, firstIncl, lastIncl, vals_.cols());
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
