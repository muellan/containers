/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_CONTAINERS_SPARSE_PAIRMAP_H_
#define AM_CONTAINERS_SPARSE_PAIRMAP_H_


#include <set>
#include <unordered_map>
#include <iterator>


namespace am {


/*****************************************************************************
 *
 * @brief   stores pairwise values v = f(i,j)
 * @details
 *
 *
 *****************************************************************************/
template<class ValueType>
class sparse_pairmap
{
	//---------------------------------------------------------------
	using index__ = std::size_t;
	//-----------------------------------------------------
	using key__ = std::pair<index__,index__>;
	//-----------------------------------------------------
	struct key_hash__ {
		index__
		operator()(const key__& k) const noexcept {
			return (k.first xor k.second);
		}
	};

	using storage__ = std::unordered_map<key__,ValueType,key_hash__>;
	using storage_iter__ = typename storage__::iterator;
	using storage_citer__ = typename storage__::const_iterator;


	//---------------------------------------------------------------
	template<class Iter>
	struct iter__
	{
		//---------------------------------------------------------------
		using iterator_category = std::forward_iterator_tag;
		using value_type = ValueType;
		using pointer = value_type*;
		using reference = value_type&;
		using difference_type =
			typename std::iterator_traits<Iter>::difference_type;


		//---------------------------------------------------------------
		constexpr
		iter__() = default;
		//-----------------------------------------------------
		explicit constexpr
		iter__(Iter it):
			it_(it)
		{}


		//---------------------------------------------------------------
		iter__&
		operator ++ () {
			++it_;
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
		operator * () -> decltype(std::declval<Iter>()->second)
		{
			return it_->second;
		}
		//-----------------------------------------------------
		auto
		operator -> () -> decltype(std::addressof(std::declval<Iter>()->second))
		{
			return std::addressof(it_->second);
		}
		//-----------------------------------------------------
		const key__&
		indices() const {
			return it_->first;
		}

		//---------------------------------------------------------------
		bool operator == (const iter__& other) const {
			return (it_ == other.it_);
		}
		bool operator != (const iter__& other) const {
			return (it_ != other.it_);
		}

	private:
		Iter it_;
	};



	//---------------------------------------------------------------
	template<class Iter>
	struct index_iter__
	{
		//---------------------------------------------------------------
		using iterator_category = std::forward_iterator_tag;
		using value_type = ValueType;
		using pointer = value_type*;
		using reference = value_type&;
		using difference_type =
			typename std::iterator_traits<Iter>::difference_type;


		//---------------------------------------------------------------
		constexpr
		index_iter__():
			it_(), end_(), idx_(0)
		{}
		//-----------------------------------------------------
		explicit constexpr
		index_iter__(Iter first):
			it_(first), end_(first), idx_(0)
		{}
		//-----------------------------------------------------
		explicit
		index_iter__(Iter first, Iter last, index__ idx):
			it_(first), end_(last), idx_(idx)
		{
			seek();
		}


		//---------------------------------------------------------------
		index_iter__&
		operator ++ () {
			++it_;
			seek();
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
		operator * () -> decltype(std::declval<Iter>()->second)
		{
			return it_->second;
		}
		//-----------------------------------------------------
		auto
		operator -> () -> decltype(std::addressof(std::declval<Iter>()->second))
		{
			return std::addressof(it_->second);
		}
		//-----------------------------------------------------
		const key__&
		indices() const {
			return it_->first;
		}

		//---------------------------------------------------------------
		bool operator == (const index_iter__& other) const {
			return (it_ == other.it_);
		}
		bool operator != (const index_iter__& other) const {
			return (it_ != other.it_);
		}

	private:
		void seek() {
			while((it_ != end_) &&
				  (it_->first.first != idx_) && (it_->first.second != idx_))
			{
				++it_;
			};
		}
		//-----------------------------------------------------
		Iter it_;
		Iter end_;
		index__ idx_;
	};



	//---------------------------------------------------------------
	template<class Iter>
	struct section__
	{
		//---------------------------------------------------------------
		struct iterator
		{
			//---------------------------------------------------------------
			using iterator_category = std::forward_iterator_tag;
			using value_type = ValueType;
			using pointer = value_type*;
			using reference = value_type&;
			using difference_type =
				typename std::iterator_traits<Iter>::difference_type;


			//---------------------------------------------------------------
			constexpr
			iterator():
				it_(), end_(), fidx_(0), lidx_(0)
			{}
			//-----------------------------------------------------
			explicit constexpr
			iterator(Iter first):
				it_(first), end_(first), fidx_(0), lidx_(0)
			{}
			//-----------------------------------------------------
			explicit
			iterator(Iter first, Iter last, index__ fidx, index__ lidx):
				it_(first), end_(last), fidx_(fidx), lidx_(lidx)
			{
				seek();
			}


			//---------------------------------------------------------------
			iterator&
			operator ++ () {
				++it_;
				seek();
				return *this;
			}
			//-----------------------------------------------------
			iterator
			operator ++ (int) {
				iterator old(*this);
				++*this;
				return old;
			}

			//---------------------------------------------------------------
			auto
			operator * () -> decltype(std::declval<Iter>()->second)
			{
				return it_->second;
			}
			//-----------------------------------------------------
			auto
			operator -> () -> decltype(std::addressof(std::declval<Iter>()->second))
			{
				return std::addressof(it_->second);
			}
			//-----------------------------------------------------
			const key__&
			indices() const {
				return it_->first;
			}

			//---------------------------------------------------------------
			bool operator == (const iterator& other) const {
				return (it_ == other.it_);
			}
			bool operator != (const iterator& other) const {
				return (it_ != other.it_);
			}

		private:
			void seek() {
				while((it_ != end_) && (
					  (it_->first.first < fidx_) ||
					  (it_->first.second > lidx_)) )
				{
					++it_;
				};
			}
			//-----------------------------------------------------
			Iter it_;
			Iter end_;
			index__ fidx_, lidx_;
		};


		//-----------------------------------------------------
		using difference_type = typename iterator::difference_type;

		//-----------------------------------------------------
		constexpr
		section__() = default;
		//-----------------------------------------------------
		explicit constexpr
		section__(Iter fst, Iter lst,
			difference_type fidx, difference_type lidx)
		:
			fst_(fst), lst_(lst), fidx_(fidx), lidx_(lidx)
		{}

		//-----------------------------------------------------
		constexpr iterator
		begin() const {
			return iterator(fst_, lst_, fidx_, lidx_);
		}
		//-----------------------------------------------------
		constexpr iterator
		end() const {
			return iterator(lst_);
		}


	private:
		Iter fst_;
		Iter lst_;
		difference_type fidx_, lidx_;
	};


public:

	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using value_type = ValueType;
	using size_type = index__;
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
		friend class sparse_pairmap;
	public:
		//-----------------------------------------------------
		memento() = default;

		//-----------------------------------------------------
		memento(const sparse_pairmap& source, size_type index):
			mem_()
		{
			backup(source,index);
		}

		//-----------------------------------------------------
		void
		backup(const sparse_pairmap& source, size_type index)
		{
			mem_.clear();

			for(const auto& x : source.vals_) {
				if((x.first.first != index) && (x.first.second != index)) {
					mem_.push_back(x);
				}
			}
		}

		//-----------------------------------------------------
		void
		restore(sparse_pairmap& target, size_type index)
		{
			//remove current values associated with 'index'
			target.erase_index(index);
			//insert old ones
			for(const auto& x : mem_) {
				target.vals_.insert(x);
			}
		}

	private:
		std::vector<typename storage__::value_type> mem_;
	};

	friend class memento;


	//---------------------------------------------------------------
	// CONSTRUCTION
	//---------------------------------------------------------------
	sparse_pairmap() = default;
	//-----------------------------------------------------
	sparse_pairmap(const sparse_pairmap&) = default;
	//-----------------------------------------------------
	sparse_pairmap(sparse_pairmap&& source):
		vals_{std::move(source.vals_)}
	{}


	//---------------------------------------------------------------
	// SETTERS
	//---------------------------------------------------------------
	sparse_pairmap& operator = (const sparse_pairmap&) = default;
	//-----------------------------------------------------
	sparse_pairmap&
	operator = (sparse_pairmap&& source) {
		vals_ = std::move(source.vals_);
		return *this;
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
		std::set<size_type> idxs;

		for(auto& x : vals_) {
			idxs.insert(x.first.first);
			idxs.insert(x.first.second);
		}

		return idxs.size();
	}
	//-----------------------------------------------------
	size_type
	min_index() const {
		if(vals_.empty()) return 0;

		size_type n = std::numeric_limits<size_type>::max();

		for(auto& x : vals_) {
			if(n < x.first.first) n = x.first.first;
			if(n < x.first.first) n = x.first.second;
		}

		return n;
	}
	//-----------------------------------------------------
	size_type
	max_index() const {
		size_type n = 0;

		for(auto& x : vals_) {
			if(n > x.first.first) n = x.first.first;
			if(n > x.first.first) n = x.first.second;
		}

		return n;
	}

	//-----------------------------------------------------
	size_type
	size() const {
		return vals_.size();
	}

	//-----------------------------------------------------
	value_type&
	operator () (size_type idx1, size_type idx2) {
		return ((idx1 < idx2) ? vals_[{idx1,idx2}] : vals_[{idx2,idx1}]);
	}
	//-----------------------------------------------------
	const value_type&
	operator () (size_type idx1, size_type idx2) const {
		return ((idx1 < idx2) ? vals_[{idx1,idx2}] : vals_[{idx2,idx1}]);
	}

	//-----------------------------------------------------
	bool
	contains(size_type idx1, size_type idx2) const {
		return ((idx1 < idx2)
			? (vals_.find({idx1,idx2}) != vals_.end())
			: (vals_.find({idx2,idx1}) != vals_.end()) );
	}

	//-----------------------------------------------------
	void
	increase_indices(size_type firstIndex, size_type n = 1) {
		if(n < 1) return;

		using backup__ = std::vector<typename storage__::value_type>;

		//backup values with occurences of indices >= 'index'
		backup__ backup;
		for(auto it = vals_.begin(); it != vals_.end();) {
			if(	(it->first.first  >= firstIndex) ||
				(it->first.second >= firstIndex) )
			{
				backup.push_back(*it);
				it = vals_.erase(it);
			} else {
				++it;
			}
		}

		//insert values with increased indices
		for(const auto& x : backup) {
			auto k = x.first;
			if(k.first  >= firstIndex) k.first  += n;
			if(k.second >= firstIndex) k.second += n;
			vals_[k] = x.second;
		}
	}


	//---------------------------------------------------------------
	void
	erase(size_type idx1, size_type idx2)
	{
		if(idx1 < idx2) {
			auto it = vals_.find({idx1,idx2});
			if(it != vals_.end()) vals_.erase(it);
		} else {
			auto it = vals_.find({idx2,idx1});
			if(it != vals_.end()) vals_.erase(it);
		}
	}
	//-----------------------------------------------------
	void
	erase_index(size_type index)
	{
		for(auto it = vals_.begin(); it != vals_.end();) {
			if(it->first.first == index || it->first.second == index) {
				it = vals_.erase(it);
			} else {
				++it;
			}
		}
	}
	//-----------------------------------------------------
	void
	erase_range(size_type firstIndex, size_type lastIndex)
	{
		if(firstIndex >= lastIndex) return;

		for(auto it = vals_.begin(); it != vals_.end();) {

			if( (it->first.first  >= firstIndex &&
				 it->first.first  <= lastIndex) ||
				(it->first.second >= firstIndex &&
				 it->first.second <= lastIndex) )
			{
				it = vals_.erase(it);
			} else {
				++it;
			}
		}
	}

	//-----------------------------------------------------
	void
	erase_index_decrease_above(size_type index) {
		using backup__ = std::vector<typename storage__::value_type>;

		//backup values with occurences of indices > 'index'
		backup__ backup;
		for(auto it = vals_.begin(); it != vals_.end();) {
			if(	(it->first.first == index) || (it->first.second == index)) {
				it = vals_.erase(it);
			}
			else if( (it->first.first > index) || (it->first.second > index)) {
				backup.push_back(*it);
				it = vals_.erase(it);
			}
			else {
				++it;
			}
		}

		//insert values with decreased indices
		for(const auto& x : backup) {
			auto k = x.first;
			if(k.first  > index) --k.first;
			if(k.second > index) --k.second;
			vals_[k] = x.second;
		}
	}
	//-----------------------------------------------------
	void
	erase_range_decrease_above(size_type firstIndex, size_type lastIndex) {
		if(firstIndex >= lastIndex) return;

		using backup__ = std::vector<typename storage__::value_type>;

		//backup values with occurences of indices > 'lastIndex'
		backup__ backup;
		for(auto it = vals_.begin(); it != vals_.end();) {

			if( (it->first.first  >= firstIndex &&
				 it->first.first  <= lastIndex) ||
				(it->first.second >= firstIndex &&
				 it->first.second <= lastIndex) )
			{
				it = vals_.erase(it);
			}
			else if( (it->first.first  > lastIndex) ||
				     (it->first.second > lastIndex) )
			{
				backup.push_back(*it);
				it = vals_.erase(it);
			}
			else {
				++it;
			}
		}

		const auto n = lastIndex - firstIndex + 1;

		//insert values with decreased indices
		for(const auto& x : backup) {
			auto k = x.first;
			if(k.first  > lastIndex) k.first  -= n;
			if(k.second > lastIndex) k.second -= n;
			vals_[k] = x.second;
		}
	}

	//-----------------------------------------------------
	void
	swap_indices(size_type idx1, size_type idx2) {
		using std::swap;

		if(idx1 == idx2) return;

		using backup__ = std::vector<typename storage__::value_type>;

		//backup and erase values with occurences of idx1 or idx2
		backup__ backup;
		for(auto it = vals_.begin(); it != vals_.end();) {
			if(	(it->first.first == idx1) || (it->first.second == idx1) ||
				(it->first.first == idx2) || (it->first.second == idx2) )
			{
				backup.push_back(*it);
				it = vals_.erase(it);
			} else {
				++it;
			}
		}

		//insert values with reversed indices
		for(const auto& x : backup) {
			auto k = x.first;

			if(k.first == idx1)
				k.first = idx2;
			else if(k.first == idx2)
				k.first = idx1;

			if(k.second == idx1)
				k.second = idx2;
			else if(k.second == idx2)
				k.second = idx1;

			operator()(k.first, k.second) = x.second;
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
		return iterator(vals_.begin());
	}
	const_iterator
	begin() const {
		return const_iterator(vals_.begin());
	}
	const_iterator
	cbegin() const {
		return const_iterator(vals_.begin());
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
	index_iterator
	begin(size_type index) {
		return index_iterator(vals_.begin(), vals_.end(), index);
	}
	const_index_iterator
	begin(size_type index) const {
		return const_index_iterator(vals_.begin(), vals_.end(), index);
	}
	const_index_iterator
	cbegin(size_type index) const {
		return const_index_iterator(vals_.begin(), vals_.end(), index);
	}

	//-----------------------------------------------------
	index_iterator
	end(size_type) {
		return index_iterator(vals_.end());
	}
	const_index_iterator
	end(size_type) const {
		return const_index_iterator(vals_.end());
	}
	const_index_iterator
	cend(size_type) const {
		return const_index_iterator(vals_.end());
	}


	//---------------------------------------------------------------
	// SECTIONS
	//---------------------------------------------------------------
	section
	subrange(size_type firstIncl, size_type lastIncl) {
		return section(vals_.begin(), vals_.end(), firstIncl, lastIncl);
	}
	const_section
	subrange(size_type firstIncl, size_type lastIncl) const {
		return const_section(vals_.begin(), vals_.end(), firstIncl, lastIncl);
	}
	const_section
	csubrange(size_type firstIncl, size_type lastIncl) const {
		return const_section(vals_.begin(), vals_.end(), firstIncl, lastIncl);
	}


private:
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
begin(sparse_pairmap<T>& m) -> decltype(m.begin())
{
	return m.begin();
}

//---------------------------------------------------------
template<class T>
inline auto
begin(const sparse_pairmap<T>& m) -> decltype(m.begin())
{
	return m.begin();
}

//---------------------------------------------------------
template<class T>
inline auto
cbegin(const sparse_pairmap<T>& m) -> decltype(m.cbegin())
{
	return m.cbegin();
}



//-------------------------------------------------------------------
template<class T>
inline auto
end(sparse_pairmap<T>& m) -> decltype(m.end())
{
	return m.end();
}

//---------------------------------------------------------
template<class T>
inline auto
end(const sparse_pairmap<T>& m) -> decltype(m.end())
{
	return m.end();
}

//---------------------------------------------------------
template<class T>
inline auto
cend(const sparse_pairmap<T>& m) -> decltype(m.cend())
{
	return m.cend();
}





}  // namespace am


#endif
