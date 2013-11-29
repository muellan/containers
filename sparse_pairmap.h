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
	template<class T>
	class iter__
	{
	public:
		//---------------------------------------------------------------
		using iterator_category = std::forward_iterator_tag;
		using iterator_type = T;
		using value_type = ValueType;
		using pointer = value_type*;
		using reference = value_type&;
		using difference_type = typename std::iterator_traits<T>::difference_type;


		//---------------------------------------------------------------
		constexpr
		iter__() = default;
		//-----------------------------------------------------
		explicit constexpr
		iter__(iterator_type it):
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
		operator * () -> decltype(std::declval<T>()->second)
		{
			return it_->second;
		}
		//-----------------------------------------------------
		auto
		operator -> () -> decltype(std::addressof(std::declval<T>()->second))
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
		iterator_type it_;
	};


	//---------------------------------------------------------------
	template<class T>
	class index_iter__
	{
	public:
		//---------------------------------------------------------------
		using iterator_category = std::forward_iterator_tag;
		using iterator_type = T;
		using value_type = ValueType;
		using pointer = value_type*;
		using reference = value_type&;
		using difference_type = typename std::iterator_traits<T>::difference_type;


		//---------------------------------------------------------------
		constexpr
		index_iter__():
			it_(), end_(), idx_(0)
		{}
		//-----------------------------------------------------
		explicit constexpr
		index_iter__(iterator_type first):
			it_(first), end_(first), idx_(0)
		{}
		//-----------------------------------------------------
		explicit
		index_iter__(iterator_type first, iterator_type last, index__ idx):
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
		operator * () -> decltype(std::declval<T>()->second)
		{
			return it_->second;
		}
		//-----------------------------------------------------
		auto
		operator -> () -> decltype(std::addressof(std::declval<T>()->second))
		{
			return std::addressof(it_->second);
		}
		//-----------------------------------------------------
		const key__&
		indices() const {
			return it_->first;
		}

		//---------------------------------------------------------------
		explicit
		operator bool() const {
			return (it_ != end_);
		}

		//-----------------------------------------------------
		constexpr index_iter__
		end() const {
			return index_iter__{end_};
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
		iterator_type it_;
		iterator_type end_;
		index__ idx_;
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
	using       local_iterator = index_iter__<storage_iter__>;
	using const_local_iterator = index_iter__<storage_citer__>;


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


	//---------------------------------------------------------------
	void
	insert_index(size_type index, const memento& mem) {
		mem.restore(*this, index);
	}


	//---------------------------------------------------------------
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
	erase_indices(size_type firstIndex, size_type lastIndex)
	{
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
	swap_indices(size_type idx1, size_type idx2) {
		using std::swap;

		if(idx1 == idx2) return;

		using backup__ = std::vector<typename storage__::value_type>;

		//backup values with occurences of idx1 or idx2
		backup__ backup;
		for(const auto& x : vals_) {
			if(	(x.first.first == idx1) || (x.first.second == idx1) ||
				(x.first.first == idx2) || (x.first.second == idx2) )
			{
				backup.push_back(x);
			}
		}
		erase_index(idx1);
		erase_index(idx2);

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
	local_iterator
	begin(size_type index) {
		return local_iterator(vals_.begin(), vals_.end(), index);
	}
	const_local_iterator
	begin(size_type index) const {
		return const_local_iterator(vals_.begin(), vals_.end(), index);
	}
	const_local_iterator
	cbegin(size_type index) const {
		return const_local_iterator(vals_.begin(), vals_.end(), index);
	}

	//-----------------------------------------------------
	local_iterator
	end(size_type) {
		return local_iterator(vals_.end());
	}
	const_local_iterator
	end(size_type) const {
		return const_local_iterator(vals_.end());
	}
	const_local_iterator
	cend(size_type) const {
		return const_local_iterator(vals_.end());
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
