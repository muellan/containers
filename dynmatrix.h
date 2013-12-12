/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_CONTAINERS_DYNMATRIX_H_
#define AM_CONTAINERS_DYNMATRIX_H_

#include <type_traits>
#include <iterator>
#include <memory>
#include <exception>


namespace am {


/*****************************************************************************
 *
 * EXCEPTIONS
 *
 *****************************************************************************/
struct dynmatrix_init_incoherent_row_sizes :
	public std::exception
{};







/*****************************************************************************
 *
 *
 *
 * TODO am::dynmatrix relies on value_type beeing CopyConstructible
 *
 *
 *
 *****************************************************************************/
template<class ValueType, class Allocator = std::allocator<ValueType>>
class dynmatrix
{
	using this_t = dynmatrix<ValueType,Allocator>;
	using alloc_traits = std::allocator_traits<Allocator>;


	/*************************************************************************
	 *
	 *
	 *************************************************************************/
	template<class T>
	class stride_iter__ :
		public std::iterator<std::random_access_iterator_tag,T>
	{
		friend class dynmatrix;

		using base_t = std::iterator<std::random_access_iterator_tag,T> ;
		using this_t = stride_iter__<T> ;

	public:
		//---------------------------------------------------------------
		using value_type = typename base_t::value_type ;
		using pointer = typename base_t::pointer ;
		using reference = typename base_t::reference ;
		using difference_type = typename base_t::difference_type ;

		//---------------------------------------------------------------
		explicit constexpr
		stride_iter__():
			base_t{},
			p_(nullptr), stride_{0}
		{}

	private:
		//---------------------------------------------------------------
		explicit constexpr
		stride_iter__(pointer p, std::size_t stride = 0):
			base_t{},
			p_(p), stride_{static_cast<difference_type>(stride)}
		{}

	public:

		//---------------------------------------------------------------
		reference operator *() const {
			return (*p_);
		}
		//-----------------------------------------------------
		pointer operator ->() const {
			return (p_);
		}
		//-----------------------------------------------------
		explicit operator bool() const {
			return (p_);
		}

		//-----------------------------------------------------
		value_type& operator [] (difference_type i) const {
			return *(p_ + (stride_ * i));
		}


		//---------------------------------------------------------------
		this_t& operator ++ () {
			p_ += stride_;
			return *this;
		}
		this_t operator ++ (int) {
			this_t old{*this};
			++*this;
			return old;
		}
		this_t& operator += (difference_type i) {
			p_ += (stride_ * i);
			return *this;
		}
		this_t operator + (difference_type i) const {
			return this_t{p_ + (stride_ * i), stride_};
		}
		//-----------------------------------------------------
		this_t& operator -- () {
			p_ -= stride_;
			return *this;
		}
		this_t operator -- (int) {
			this_t old{*this};
			--*this;
			return old;
		}
		this_t& operator -= (difference_type i) {
			p_ -= (stride_ * i);
			return *this;
		}
		this_t operator - (difference_type i) const {
			return this_t{p_ - (stride_ * i), stride_};
		}


		//---------------------------------------------------------------
		bool operator ==(const this_t& other) {
			return (p_ == other.p_);
		}
		bool operator !=(const this_t& other) {
			return (p_ != other.p_);
		}
		bool operator < (const this_t& other) {
			return (p_ < other.p_);
		}
		bool operator <=(const this_t& other) {
			return (p_ <= other.p_);
		}
		bool operator > (const this_t& other) {
			return (p_ > other.p_);
		}
		bool operator >=(const this_t& other) {
			return (p_ >= other.p_);
		}

	private:
		pointer p_;
		difference_type stride_;
	};


	//---------------------------------------------------------------
	template<class T>
	struct section__
	{
		//-----------------------------------------------------
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using difference_type = std::ptrdiff_t;

		//--------------------------------------------------------
		class iterator
		{
		public:
			//-----------------------------------------------------
			using iterator_category = std::forward_iterator_tag;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using difference_type = std::ptrdiff_t;

			//-----------------------------------------------------
			explicit constexpr
			iterator(
				pointer p = nullptr,
				difference_type length = 0, difference_type stride = 0) noexcept
			:
				p_{p}, count_(0), length_(length), stride_(stride)
			{}


			//-----------------------------------------------------
			reference operator *() const {
				return (*p_);
			}
			//-----------------------------------------------------
			pointer operator ->() const noexcept {
				return (p_);
			}

			//-----------------------------------------------------
			iterator& operator ++ () {
				++p_;
				++count_;
				if(count_ >= length_) {
					count_ = 0;
					p_ += stride_;
				}
				return *this;
			}
			//-----------------------------------------------------
			iterator operator ++ (int) {
				auto old(*this);
				++*this;
				return old;
			}

			//-----------------------------------------------------
			bool operator ==(const iterator& other) {
				return (p_ == other.p_);
			}
			bool operator !=(const iterator& other) {
				return (p_ != other.p_);
			}

		private:
			pointer p_;
			difference_type count_, length_, stride_;
		};


		//---------------------------------------------------------------
		explicit constexpr
		section__(
			pointer pbeg = nullptr, pointer pend = nullptr,
			difference_type length = 0, difference_type stride = 0) noexcept
		:
			pbeg_{pbeg}, pend_{pend}, length_(length), stride_(stride)
		{}

		//-----------------------------------------------------
		constexpr iterator
		begin() const noexcept {
			return iterator{pbeg_, length_, stride_};
		}

		//-----------------------------------------------------
		constexpr iterator
		end() const noexcept {
			return iterator{pend_};
		}


	private:
		pointer pbeg_, pend_;
		difference_type length_, stride_;
	};


public:
	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using value_type     = ValueType;
	using allocator_type = Allocator;
	//-----------------------------------------------------
	using reference       = ValueType&;
	using const_reference = const ValueType&;
	//-----------------------------------------------------
	using pointer         = ValueType*;
	using const_pointer   = const ValueType*;
	//-----------------------------------------------------
	using iterator        = pointer;
	using const_iterator  = const_pointer;
	//-----------------------------------------------------
	using reverse_iterator       = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	//-----------------------------------------------------
	using row_iterator       = pointer;
	using const_row_iterator = const_pointer;
	//-----------------------------------------------------
	using col_iterator       = stride_iter__<value_type>;
	using const_col_iterator = stride_iter__<const value_type>;
	//-----------------------------------------------------
	using section            = section__<value_type>;
	using const_section      = section__<const value_type>;
	//-----------------------------------------------------
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;


	//---------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION
	//---------------------------------------------------------------
	/// @brief default constructor
	explicit constexpr
	dynmatrix():
		rows_{0}, cols_{0},
		first_{nullptr}, last_{nullptr}, memEnd_{nullptr},
		alloc_{}
	{}

	//-----------------------------------------------------
	/// @brief initializer list constructor
	dynmatrix(std::initializer_list<value_type> il):
		rows_{static_cast<size_type>(il.size() > 0 ? 1 : 0)},
		cols_{il.size()},
		first_{nullptr}, last_{nullptr}, memEnd_{nullptr},
		alloc_{}
	{
		//initial capacity will be exactly the same as size
		auto totSize = rows_ * cols_;
		if(totSize > 0) {
			//reserve memory
			first_ = alloc_traits::allocate(alloc_,totSize);
			last_ = first_;
			memEnd_ = first_ + totSize;
			//contruct elements in-place
			for(auto i = il.begin(), e = il.end(); i != e; ++i) {
				alloc_traits::construct(alloc_, last_, *i);
				++last_;
			}
		}
	}

	//-----------------------------------------------------
	/// @brief initializer list constructor
	dynmatrix(std::initializer_list<std::initializer_list<value_type>> il):
		rows_{il.size()},
		cols_{rows_ > 0 ? (il.begin())->size() : size_type(0)},
		first_{nullptr}, last_{nullptr}, memEnd_{nullptr},
		alloc_{}
	{
		//check row sizes
		#ifdef AM_USE_EXCEPTIONS
		for(auto i = il.begin(), e = il.end(); i != e; ++i) {
			if(i->size() != cols_) {
				throw dynmatrix_init_incoherent_row_sizes{};
			}
		}
		#endif
		//initial capacity will be exactly the same as size
		auto totSize = rows_ * cols_;
		if(totSize > 0) {
			//reserve memory
			first_ = alloc_traits::allocate(alloc_,totSize);
			last_ = first_;
			memEnd_ = first_ + totSize;
			//contruct elements in-place
			for(auto i = il.begin(), e = il.end(); i != e; ++i) {
				for(auto i2 = i->begin(), e2 = i->end(); i2 != e2; ++i2) {
					alloc_traits::construct(alloc_, last_, *i2);
					++last_;
				}
			}
		}
	}

	//-----------------------------------------------------
	/// @brief copy constructor
	/// @details delegates to special private constructor
	dynmatrix(const dynmatrix& o):
		dynmatrix(o, o.size())
	{}

	//-----------------------------------------------------
	/// @brief move constructor
	dynmatrix(dynmatrix&& o):
		rows_{o.rows_}, cols_{o.cols_},
		first_{o.first_}, last_{o.last_}, memEnd_{o.memEnd_},
		alloc_{o.alloc_}
	{
		o.cols_ = 0;
		o.rows_ = 0;
		o.first_ = nullptr;
		o.last_ = nullptr;
		o.memEnd_ = nullptr;
	}

	//---------------------------------------------------------------
	~dynmatrix() {
		mem_erase();
	}


	//---------------------------------------------------------------
	// ASSIGNMENT
	//---------------------------------------------------------------
	dynmatrix&
	operator = (const dynmatrix& source) {
		if(this == &source) return *this;
		this_t temp{source};
		swap(*this,temp);
		return *this;
	}
	//-----------------------------------------------------
	dynmatrix&
	operator = (dynmatrix&& source) {
		swap(*this, source);
		return *this;
	}

	//---------------------------------------------------------------
	void
	fill(const value_type& value)
	{
		std::fill(begin(), end(), value);
	}
	//-----------------------------------------------------
	void
	fill_row(size_type index, const value_type& value)
	{
		std::fill(begin_row(index), end_row(index), value);
	}
	//-----------------------------------------------------
	void
	fill_col(size_type index, const value_type& value)
	{
		std::fill(begin_col(index), end_col(index), value);
	}


	//---------------------------------------------------------------
	// SET SIZE
	//---------------------------------------------------------------
	void
	rows(size_type numRows)
	{
		if(rows_ == numRows) return;

		if(numRows == 0) {
			clear();
		} else {
			if(cols_ < 1) cols_ = 1;
			mem_resize(numRows * cols_);
			rows_ = numRows;
		}
	}
	//-----------------------------------------------------
	void
	rows(size_type numRows, const value_type& value)
	{
		if(rows_ == numRows) return;

		if(numRows == 0) {
			clear();
		} else {
			if(cols_ < 1) cols_ = 1;
			mem_resize(numRows * cols_, value);
			rows_ = numRows;
		}
	}

	//-----------------------------------------------------
	void
	cols(size_type numCols)
	{
		if(cols_ == numCols) return;

		if(numCols < cols_) {
			mem_erase_cols(numCols, cols_-1);
		}
		else {
			if(rows_ < 1) rows_ = 1;

			size_type oldCols = cols_;
			mem_insert_cols(cols_, numCols - cols_);
		}
	}
	//-----------------------------------------------------
	void
	cols(size_type numCols, const value_type& value)
	{
		if(cols_ == numCols) return;

		if(numCols < cols_) {
			mem_erase_cols(numCols, cols_-1);
		}
		else {
			if(rows_ < 1) rows_ = 1;

			auto oldCols = cols_;
			mem_insert_cols(cols_, numCols - cols_);

			for(size_type i = oldCols; i < numCols; ++i)
				fill_col(i, value);
		}
	}

	//-----------------------------------------------------
	void
	resize(size_type numRows, size_type numCols)
	{
		if((numRows < 1) || (numCols < 1)) {
			clear();
		}
		else if((numRows == 1) || (numCols == 1)) {
			dynmatrix temp(numRows,numCols);
			swap(*this,temp);
		}
		else {
			reserve(numRows, numCols);
			cols(numCols);
			rows(numRows);
		}
	}
	//-----------------------------------------------------
	void
	resize(size_type numRows, size_type numCols, const value_type& value)
	{
		if((numRows < 1) || (numCols < 1)) {
			clear();
		}
		else if((numRows == 1) || (numCols == 1)) {
			dynmatrix temp(numRows,numCols,value);
			swap(*this,temp);
		}
		else {
			reserve(numRows, numCols);
			cols(numCols, value);
			rows(numRows, value);
		}
	}

	//-----------------------------------------------------
	void
	reserve(size_type numRows, size_type numCols)
	{
		mem_reserve(numRows * numCols);
	}


	//---------------------------------------------------------------
	// INSERT
	//---------------------------------------------------------------
	/// @brief returns valid iterator to the newly inserted col
	col_iterator
	insert_col(size_type index) {
		insert_cols(index, 1);
		return begin_col(index);
	}
	//-----------------------------------------------------
	/// @brief returns valid iterator to the newly inserted col
	col_iterator
	insert_col(size_type index, const value_type& value) {
		insert_cols(index, 1, value);
		return begin_col(index);
	}

	//-----------------------------------------------------
	/// @brief returns valid iterator to the newly inserted row
	row_iterator
	insert_row(size_type index) {
		insert_rows(index, 1);
		return begin_row(index);
	}
	//-----------------------------------------------------
	/// @brief returns valid iterator to the newly inserted row
	row_iterator
	insert_row(size_type index, const value_type& value) {
		insert_rows(index, 1, value);
		return begin_row(index);
	}

	//-----------------------------------------------------
	/// @brief returns valid iterator to the first of the newly inserted cols
	col_iterator
	insert_cols(size_type index, size_type quantity)
	{
		if(rows_ < 1) {
			resize(1,quantity);
			begin_col(0);
		}
		else {
			mem_insert_cols(index,quantity);
		}
		return begin_col(index);
	}
	//-----------------------------------------------------
	/// @brief returns valid iterator to the first of the newly inserted cols
	col_iterator
	insert_cols(size_type index, size_type quantity, const value_type& value)
	{
		if(rows_ < 1) {
			resize(1,quantity,value);
		}
		else {
			mem_insert_cols(index,quantity);

			for(auto i = index, e = index+quantity; i < e; ++i)
				fill_col(i, value);
		}
		return begin_col(index);
	}

	//-----------------------------------------------------
	/// @brief returns valid iterator to the first of the newly inserted rows
	row_iterator
	insert_rows(size_type index, size_type quantity)
	{
		if(cols_ < 1) {
			resize(quantity,1);
		}
		else {
			mem_insert_rows(index,quantity);
		}
		return begin_row(index);
	}
	//-----------------------------------------------------
	/// @brief returns valid iterator to the first of the newly inserted rows
	row_iterator
	insert_rows(size_type index, size_type quantity, const value_type& value)
	{
		if(cols_ < 1) {
			resize(quantity,1,value);
		}
		else {
			mem_insert_rows(index,quantity);

			for(auto i = index, e = index+quantity; i < e; ++i)
				fill_row(i, value);
		}
		return begin_row(index);
	}


	//---------------------------------------------------------------
	// REMOVE
	//---------------------------------------------------------------
	void
	erase_col(size_type idx) {
		mem_erase_cols(idx,idx);
	}
	//-----------------------------------------------------
	void
	erase_row(size_type idx) {
		mem_erase_rows(idx,idx);
	}

	//-----------------------------------------------------
	void
	erase_cols(size_type first, size_type last) {
		mem_erase_cols(first,last);
	}
	//-----------------------------------------------------
	void
	erase_rows(size_type first, size_type last) {
		mem_erase_rows(first,last);
	}

	//-----------------------------------------------------
	void
	clear() {
		rows_ = 0;
		cols_ = 0;
		mem_destroy_content();
	}
	//-----------------------------------------------------
	void
	shrink_to_fit() {
		if(!last_) return;
		using std::distance;
		alloc_traits::deallocate(alloc_, last_, distance(last_, memEnd_));
		memEnd_ = last_;
	}


	//---------------------------------------------------------------
	// ALTER DATA
	//---------------------------------------------------------------
	void
	swap_rows(size_type r1, size_type r2) {
		std::swap_ranges(begin_row(r1), end_row(r1), begin_row(r2));
	}
	//-----------------------------------------------------
	void
	swap_cols(size_type c1, size_type c2) {
		std::swap_ranges(begin_col(c1), end_col(c1), begin_col(c2));
	}


	//---------------------------------------------------------------
	// ACCESS
	//---------------------------------------------------------------
	reference
	operator () (size_type row, size_type col) {
		return first_[row*cols_+col];
	}
	//-----------------------------------------------------
	const_reference
	operator () (size_type row, size_type col) const {
		return first_[row*cols_+col];
	}

	//-----------------------------------------------------
	size_type
	row(const_iterator it) const {
		using std::distance;
		return static_cast<size_type>(distance(begin(), it) / cols_);
	}
	//-----------------------------------------------------
	size_type
	col(const_iterator it) const {
		using std::distance;
		return static_cast<size_type>(distance(begin(), it) % cols_);
	}


	//---------------------------------------------------------------
	// SIZE PROPERTIES
	//---------------------------------------------------------------
	size_type
	rows() const noexcept {
		return rows_;
	}
	size_type
	cols() const noexcept {
		return cols_;
	}
	//-----------------------------------------------------
	size_type
	size() const {
		using std::distance;
		return distance(first_, last_);
	}
	//-----------------------------------------------------
	size_type
	max_size() const {
		return alloc_traits::max_size(alloc_);
	}
	//-----------------------------------------------------
	size_type
	capacity() const {
		using std::distance;
		return distance(first_, memEnd_);
	}
	//-----------------------------------------------------
	bool
	empty() const noexcept {
		return (first_ == last_);
	}
	//-----------------------------------------------------
	bool
	is_square() const noexcept {
		return (rows_ == cols_);
	}


	//---------------------------------------------------------------
	// SEQUENTIAL ITERATORS
	//---------------------------------------------------------------
	iterator
	begin() {
		return first_;
	}
	const_iterator
	begin() const {
		return first_;
	}
	const_iterator
	cbegin() const {
		return first_;
	}
	//-----------------------------------------------------
	friend iterator
	begin(this_t& m) {
		return m.first_;
	}
	friend const_iterator
	begin(const this_t& m) {
		return m.first_;
	}
	friend const_iterator
	cbegin(const this_t& m) {
		return m.first_;
	}
	//-----------------------------------------------------
	iterator
	begin(size_type row, size_type col) {
		return ptr(row,col);
	}
	//-----------------------------------------------------
	const_iterator
	begin(size_type row, size_type col) const {
		return ptr(row,col);
	}
	//-----------------------------------------------------
	const_iterator
	cbegin(size_type row, size_type col) const {
		return ptr(row,col);
	}

	//-----------------------------------------------------
	iterator
	end() {
		return last_;
	}
	const_iterator
	end() const {
		return last_;
	}
	const_iterator
	cend() const {
		return last_;
	}
	//-----------------------------------------------------
	iterator
	end(this_t& m) {
		return m.last_;
	}
	const_iterator
	end(const this_t& m) {
		return m.last_;
	}
	const_iterator
	cend(const this_t& m) {
		return m.last_;
	}


	//---------------------------------------------------------------
	// REVERSE SEQUENTIAL ITERATORS
	//---------------------------------------------------------------
	reverse_iterator
	rbegin() {
		return (last_);
	}
	const_reverse_iterator
	rbegin() const {
		return (last_);
	}
	const_reverse_iterator
	crbegin() const {
		return (last_);
	}
	//-----------------------------------------------------
	friend reverse_iterator
	rbegin(this_t& m) {
		return (m.last_);
	}
	friend const_reverse_iterator
	rbegin(const this_t& m) {
		return (m.last_);
	}
	friend const_reverse_iterator
	crbegin(const this_t& m) {
		return (m.last_);
	}

	//-----------------------------------------------------
	reverse_iterator
	rend() {
		return first_;
	}
	const_reverse_iterator
	rend() const {
		return first_;
	}
	const_reverse_iterator
	crend() const {
		return first_;
	}
	//-----------------------------------------------------
	friend reverse_iterator
	rend(this_t& m) {
		return m.first_;
	}
	friend const_reverse_iterator
	rend(const this_t& m) {
		return m.first_;
	}
	friend const_reverse_iterator
	crend(const this_t& m) {
		return m.first_;
	}


	//---------------------------------------------------------------
	// ROW ITERATORS
	//---------------------------------------------------------------
	row_iterator
	begin_row(size_type row) {
		return ptr(row,0);
	}
	//-----------------------------------------------------
	const_row_iterator
	begin_row(size_type row) const {
		return ptr(row,0);
	}
	//-----------------------------------------------------
	const_row_iterator
	cbegin_row(size_type row) const {
		return ptr(row,0);
	}

	//-----------------------------------------------------
	row_iterator
	end_row(size_type row) {
		return ptr(row+1,0);
	}
	//-----------------------------------------------------
	const_row_iterator
	end_row(size_type row) const {
		return ptr(row+1,0);
	}
	//-----------------------------------------------------
	const_row_iterator
	cend_row(size_type row) const {
		return ptr(row+1,0);
	}


	//---------------------------------------------------------------
	// COLUMN ITERATORS
	//---------------------------------------------------------------
	col_iterator
	begin_col(size_type col) {
		return col_iterator{first_ + col, cols_};
	}
	//-----------------------------------------------------
	const_col_iterator
	begin_col(size_type col) const {
		return const_col_iterator{first_ + col, cols_};
	}
	//-----------------------------------------------------
	const_col_iterator
	cbegin_col(size_type col) const {
		return const_col_iterator{first_ + col, cols_};
	}

	//-----------------------------------------------------
	col_iterator
	end_col(size_type col) {
		return col_iterator{first_ + col + size()};
	}
	//-----------------------------------------------------
	const_col_iterator
	end_col(size_type col) const {
		return const_col_iterator{first_ + col + size()};
	}
	//-----------------------------------------------------
	const_col_iterator
	cend_col(size_type col) const {
		return const_col_iterator{first_ + col + size()};
	}


	//---------------------------------------------------------------
	// SECTIONS
	//---------------------------------------------------------------
	section
	subrange(
		size_type firstRow, size_type firstCol,
		size_type lastRow,  size_type lastCol)
	{
		const auto stride = difference_type(cols_ -  lastCol - 1 + firstCol);

		return section{
			ptr(firstRow,firstCol),
			ptr(lastRow,lastCol) + stride + 1,
			difference_type(lastCol - firstCol + 1),
			stride };
	}
	//-----------------------------------------------------
	const_section
	subrange(
		size_type firstRow, size_type firstCol,
		size_type lastRow,  size_type lastCol) const
	{
		const auto stride = difference_type(cols_ -  lastCol - 1 + firstCol);

		return const_section{
			ptr(firstRow,firstCol),
			ptr(lastRow,lastCol) + stride + 1,
			difference_type(lastCol - firstCol + 1),
			stride };
	}
	//-----------------------------------------------------
	const_section
	csubrange(
		size_type firstRow, size_type firstCol,
		size_type lastRow,  size_type lastCol) const
	{
		const auto stride = difference_type(cols_ -  lastCol - 1 + firstCol);

		return const_section{
			ptr(firstRow,firstCol),
			ptr(lastRow,lastCol) + stride + 1,
			difference_type(lastCol - firstCol + 1),
			stride };
	}


	//---------------------------------------------------------------
	friend void
	swap(dynmatrix& a, dynmatrix& b) noexcept
	{
		using std::swap;

		swap(a.rows_,   b.rows_);
		swap(a.cols_,   b.cols_);
		swap(a.first_,  b.first_);
		swap(a.last_,   b.last_);
		swap(a.memEnd_, b.memEnd_);
		swap(a.alloc_,  b.alloc_);
	}


	//---------------------------------------------------------------
	const allocator_type&
	get_allocator() const {
		return alloc_;
	}


	//---------------------------------------------------------------
	// I/O
	//---------------------------------------------------------------
	template<class Ostream>
	inline friend Ostream&
	operator << (Ostream& os, const this_t& o)
	{
		os << o.rows_ <<' '<< o.cols_ << '\n';
		if(o.rows_ < 1 || o.cols_ < 1) return os;
		for(size_type r = 0; r < o.rows_-1; ++r) {
			for(size_type c = 0; c < o.cols_-1; ++c)
				os << o.first_[r*o.cols_+c] << ' ';
			os << o.first_[(r+1)*o.cols_-1] << '\n';
		}
		for(size_type c = 0; c < o.cols_-1; ++c)
			os << o.first_[(o.rows_-1)*o.cols_+c] << ' ';
		os << o.first_[o.rows_*o.cols_-1];
		return os;
	}


private:
	//---------------------------------------------------------------
	explicit
	dynmatrix(const dynmatrix& o, size_type capacity):
		rows_{o.rows_}, cols_{o.cols_},
		first_{nullptr}, last_{nullptr}, memEnd_{nullptr},
		alloc_{alloc_traits::select_on_container_copy_construction(o.alloc_)}
	{
		if(capacity > 0) {
			first_ = alloc_traits::allocate(alloc_,capacity);
			memEnd_ = first_ + capacity;
			last_ = first_;

			for(auto s = o.first_; s < o.last_; ++s, ++last_)
				alloc_traits::construct(alloc_, last_, *s);
		}
	}
	//-----------------------------------------------------
	template<class... Args>
	explicit
	dynmatrix(size_type rows, size_type cols, Args&&... args):
		rows_{rows}, cols_{cols},
		first_{nullptr}, last_{nullptr}, memEnd_{nullptr},
		alloc_{}
	{
		//initial capacity will be exactly the same as size
		auto totSize = rows_ * cols_;
		if(totSize > 0) {
			//reserve memory
			first_ = alloc_traits::allocate(alloc_,totSize);
			memEnd_ = first_ + totSize;
			//default-contruct elements in-place
			for(last_ = first_; last_ < memEnd_; ++last_)
				alloc_traits::construct(alloc_, last_,
					std::forward<Args>(args)...);
		}
	}

	//---------------------------------------------------------------
	void
	mem_destroy_content() {
		if(!first_ || !last_) return;

		--last_;
		for(; last_ >= first_; --last_) {
			alloc_traits::destroy(alloc_, last_);
		}
		++last_;
	}
	//---------------------------------------------------------------
	void
	mem_reserve(size_type newSize)
	{
		//grow capacity if needed (copies old values to new memory)
		if(newSize > capacity()) {
			auto temp = this_t(*this, newSize);
			swap(*this, temp);
		}
	}
	//---------------------------------------------------------------
	template<class... Args>
	void
	mem_reserve_least(size_type newSize, Args&&... args)
	{
		if(newSize > capacity()) {
			auto temp = this_t(*this, 1.5 * newSize);
			swap(*this, temp);
		}
		//construct new elements if neccessary
		for(auto e = first_ + newSize; last_ < e; ++last_) {
			alloc_traits::construct(alloc_, last_, std::forward<Args>(args)...);
		}
	}

	//---------------------------------------------------------------
	void
	mem_resize_destroy(size_type newSize) {
		//destroy elements no longer needed
		for(auto e = first_ + newSize; last_ > e; --last_) {
			alloc_traits::destroy(alloc_, last_);
		}
	}

	//---------------------------------------------------------------
	template<class... Args>
	void
	mem_resize(size_type newSize, Args&&... args)
	{
		if(newSize == size()) return;

		if(newSize < size()) {
			mem_resize_destroy(newSize);
		}
		else { //newSize > size()
			mem_reserve(newSize);

			//(default-)construct new elements if neccessary
			for(auto e = first_ + newSize; last_ < e; ++last_) {
				alloc_traits::construct(alloc_, last_,
					std::forward<Args>(args)...);
			}
		}
	}
	//-----------------------------------------------------
	void
	mem_erase() {
		mem_destroy_content();

		if(first_) {
			alloc_traits::deallocate(alloc_, first_, capacity());
		}

		cols_ = 0;
		rows_ = 0;
		first_ = nullptr;
		last_ = nullptr;
		memEnd_ = nullptr;
	}


	//---------------------------------------------------------------
	void
	mem_erase_cols(size_type first, size_type last) {
		size_type quantity = last - first + 1;

		if(quantity >= cols_) {
			clear();
		}
		else {
			//move elements towards begin
			pointer tgt = first_ + first;
			const_pointer src = static_cast<const_pointer>(tgt);

			for(size_type r = 0; r < rows_-1; ++r) {
				src += quantity;
				for(size_type c = 0; c < cols_-quantity; ++c) {
					*tgt = *src;
					++tgt;
					++src;
				}
			}
			src += quantity;
			for(size_type c = first; c < cols_-quantity; ++c) {
				*tgt = *src;
				++tgt;
				++src;
			}

			//destroy content of remaining unused columns
			for(size_type i = 0, e = quantity * rows_; i < e; ++i) {
				--last_;
				alloc_traits::destroy(alloc_, last_);
			}
			cols_ -= quantity;
		}
	}

	//-----------------------------------------------------
	void
	mem_erase_rows(size_type first, size_type last) {
		size_type quantity = last - first + 1;

		if(quantity >= rows_) {
			clear();
		}
		else {
			//move elements towards begin
			pointer tgt = first_ + first*cols_;
			const_pointer src = first_ + (first+quantity)*cols_;

			for(size_type i = 0; i < (rows_-first-quantity)*cols_; ++i) {
				*tgt = *src;
				++tgt;
				++src;
			}

			//destroy content of remaining unused rows
			for(size_type i = 0, e = quantity * cols_; i < e; ++i) {
				--last_;
				alloc_traits::destroy(alloc_, last_);
			}
			rows_ -= quantity;
		}
	}


	//---------------------------------------------------------------
	/// @brief returns valid iterator to the first of the newly inserted cols
	void
	mem_insert_cols(size_type index, size_type quantity)
	{
		size_type oldSize = size();
		mem_reserve_least(rows_*(cols_+quantity));

		//move elements towards back
		const_pointer src = first_ + oldSize - 1;
		pointer tgt = last_ - 1;
		for(size_type i = 0; i < cols_-index; ++i, --tgt, --src) {
			*tgt = *src;
		}

		for(size_type r = 0; r < rows_-1; ++r) {
			tgt -= quantity;
			for(size_type c = 0; c < cols_; ++c,--tgt, --src) {
				*tgt = *src;
			}
		}
		cols_ += quantity;
	}

	//-----------------------------------------------------
	/// @brief returns valid iterator to the first of the newly inserted rows
	void
	mem_insert_rows(size_type index, size_type quantity)
	{
		size_type oldSize = size();
		mem_reserve_least(cols_*(rows_+quantity));

		//move elements towards back
		const_pointer src = first_ + oldSize - 1;
		const_pointer pidx = first_ + (index * cols_);
		for(pointer tgt = last_-1; tgt >= pidx; --tgt, --src) {
			*tgt = *src;
		}

		rows_ += quantity;
	}

	//---------------------------------------------------------------
	pointer
	ptr(size_type row, size_type col) const
	{
		return (first_ + row*cols_ + col);
	}


	//---------------------------------------------------------------
	// DATA MEMBERS
	//---------------------------------------------------------------
	size_type rows_ = 0;
	size_type cols_ = 0;
	pointer first_ = nullptr;
	pointer last_ = nullptr;
	pointer memEnd_ = nullptr;
	allocator_type alloc_;
};


}  // namespace am


#endif
