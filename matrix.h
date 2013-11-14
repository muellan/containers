/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_CONTAINERS_MATRIX_H_
#define AM_CONTAINERS_MATRIX_H_

#include <type_traits>
#include <iterator>


namespace am {


/*****************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *****************************************************************************/
template<
	class ValueType,
	std::size_t nrows,
	std::size_t ncols = nrows
>
class matrix
{
	using this_t = matrix<ValueType,nrows,ncols>;


	/*************************************************************************
	 *
	 *
	 *************************************************************************/
	template<class T, std::size_t stride>
	class stride_iter_t :
		public std::iterator<std::random_access_iterator_tag,T>
	{
		using base_t = std::iterator<std::random_access_iterator_tag,T> ;
		using this_t = stride_iter_t<T,stride>;

	public:
		//---------------------------------------------------------------
		using value_type = typename base_t::value_type ;
		using pointer = typename base_t::pointer ;
		using reference = typename base_t::reference ;
		using difference_type = typename base_t::difference_type ;

		//---------------------------------------------------------------
		explicit constexpr
		stride_iter_t(pointer p = nullptr):
			base_t{}, p_(p)
		{}


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
			return *(p_ + (stride * i));
		}


		//---------------------------------------------------------------
		this_t& operator ++ () {
			p_ += stride;
			return *this;
		}
		this_t operator ++ (int) {
			this_t old(*this);
			++*this;
			return old;
		}
		this_t& operator += (difference_type i) {
			p_ += (stride * i);
			return *this;
		}
		this_t operator + (difference_type i) const {
			return this_t{p_ + (stride * i)};
		}
		//-----------------------------------------------------
		this_t& operator -- () {
			p_ -= stride;
			return *this;
		}
		this_t operator -- (int) {
			this_t old(*this);
			--*this;
			return old;
		}
		this_t& operator -= (difference_type i) {
			p_ -= (stride * i);
			return *this;
		}
		this_t operator - (difference_type i) const {
			return this_t{p_ - (stride * i)};
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
	};




	/*************************************************************************
	 *
	 *
	 *************************************************************************/
	template<class T>
	class block_iter_t :
		public std::iterator<std::forward_iterator_tag,T>
	{
		using base_t = std::iterator<std::forward_iterator_tag,T> ;
		using this_t = block_iter_t<T> ;

	public:

		//---------------------------------------------------------------
		using value_type = typename base_t::value_type ;
		using pointer = typename base_t::pointer ;
		using reference = typename base_t::reference ;
		using difference_type = typename base_t::difference_type ;

		//---------------------------------------------------------------
		explicit constexpr
		block_iter_t(
			pointer p = nullptr, pointer pend = nullptr,
			difference_type length = 0, difference_type stride = 0)
		:
			base_t{},
			p_{p}, pend_{pend}, count_(0), length_(length), stride_(stride)
		{}


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
			return (p_ != pend_);
		}

		//---------------------------------------------------------------
		this_t& operator ++ () {
			++p_;
			++count_;
			if(count_ >= length_) {
				count_ = 0;
				p_ += stride_;
			}
			return *this;
		}
		//-----------------------------------------------------
		this_t operator ++ (int) {
			this_t old(*this);
			++*this;
			return old;
		}


		//---------------------------------------------------------------
		constexpr block_iter_t
		end() const {
			return block_iter_t(pend_);
		}


		//---------------------------------------------------------------
		bool operator ==(const this_t& other) {
			return ((this == &other) ? true : (p_ == other.p_));
		}
		bool operator !=(const this_t& other) {
			return ((this == &other) ? (p_ != pend_) : (p_ != other.p_));
		}

	private:
		pointer p_, pend_;
		difference_type count_, length_, stride_;
	};



public:
	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using value_type      = ValueType;
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
	using row_iterator         = pointer;
	using const_row_iterator   = const_pointer;
	//-----------------------------------------------------
	using col_iterator         = stride_iter_t<value_type,ncols>;
	using const_col_iterator   = stride_iter_t<const value_type,ncols>;
	//-----------------------------------------------------
	using diag_iterator        = stride_iter_t<value_type,ncols+1>;
	using const_diag_iterator  = stride_iter_t<const value_type,ncols+1>;
	//-----------------------------------------------------
	using block_iterator       = block_iter_t<value_type>;
	using const_block_iterator = block_iter_t<const value_type>;
	//-----------------------------------------------------
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;


	//---------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION
	//---------------------------------------------------------------
	/// @brief default constructor
	constexpr matrix() = default;

	//-----------------------------------------------------
	/// @brief initializer list constructor
	matrix(std::initializer_list<std::initializer_list<value_type>> il):
		m_{}
	{
		size_type totSize = std::min(il.size(),nrows) *
			std::min((il.begin())->size(),ncols);

		if(totSize > 0) {
			iterator mi = begin();
			for(auto i = il.begin(), e = il.end(); i != e; ++i) {
				std::copy(i->begin(), i->end(), mi);
				mi += ncols;
			}
		}
	}

	//-----------------------------------------------------
	/// @brief copy constructor
	matrix(const matrix&) = default;

	//-----------------------------------------------------
	/// @brief move constructor
	matrix(matrix&&) = default;

	//---------------------------------------------------------------
	~matrix() = default;


	//---------------------------------------------------------------
	// ASSIGNMENT
	//---------------------------------------------------------------
	matrix& operator = (const matrix&) = default;
	matrix& operator = (matrix&&) = default;

	//---------------------------------------------------------------
	void
	fill(const value_type& value) {
		std::fill(begin(), end(), value);
	}
	//-----------------------------------------------------
	template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
	void
	fill_diag(const value_type& value) {
		std::fill(diag_begin(), diag_end(), value);
	}
	//-----------------------------------------------------
	void
	fill_row(size_type index, const value_type& value)
	{
		std::fill(row_begin(index), row_end(index), value);
	}
	//-----------------------------------------------------
	void
	fill_col(size_type index, const value_type& value)
	{
		std::fill(col_begin(index), col_end(index), value);
	}


	//---------------------------------------------------------------
	// ALTER DATA
	//---------------------------------------------------------------
	void
	swap_rows(size_type r1, size_type r2) {
		std::swap_ranges(row_begin(r1), row_end(r1), row_begin(r2));
	}
	//-----------------------------------------------------
	void
	swap_cols(size_type c1, size_type c2) {
		std::swap_ranges(col_begin(c1), col_end(c1), col_begin(c2));
	}


	//---------------------------------------------------------------
	// ACCESS
	//---------------------------------------------------------------
	pointer
	data() {
		return static_cast<pointer>(m_);
	}
	//-----------------------------------------------------
	const_pointer
	data() const {
		return static_cast<const value_type*>(m_);
	}
	//-----------------------------------------------------
	reference
	operator () (size_type row, size_type col) {
		return m_[row][col];
	}
	//-----------------------------------------------------
	const_reference
	operator () (size_type row, size_type col) const {
		return m_[row][col];
	}


	//---------------------------------------------------------------
	// SIZE PROPERTIES
	//---------------------------------------------------------------
	static constexpr size_type
	rows() noexcept {
		return nrows;
	}
	static constexpr size_type
	cols() noexcept {
		return ncols;
	}
	//-----------------------------------------------------
	static constexpr size_type
	size() noexcept {
		return (nrows*ncols);
	}
	//-----------------------------------------------------
	static constexpr size_type
	max_size() noexcept {
		return (nrows*ncols);
	}
	//-----------------------------------------------------
	static constexpr
	bool
	is_square() noexcept {
		return (nrows == ncols);
	}


	//---------------------------------------------------------------
	// SEQUENTIAL ITERATORS
	//---------------------------------------------------------------
	iterator
	begin() noexcept {
		return iterator{std::addressof(m_[0][0])};
	}
	const_iterator
	begin() const noexcept {
		return const_iterator{std::addressof(m_[0][0])};
	}
	const_iterator
	cbegin() const noexcept {
		return const_iterator{std::addressof(m_[0][0])};
	}
	//-----------------------------------------------------
	inline friend iterator
	begin(this_t& i) noexcept {
		return i.begin();
	}
	inline friend const_iterator
	begin(const this_t& i) noexcept {
		return i.begin();
	}
	inline friend const_iterator
	cbegin(const this_t& i) noexcept {
		return i.cbegin();
	}
	//-----------------------------------------------------
	iterator
	begin(size_type row, size_type col) {
		return m_[row][col];
	}
	//-----------------------------------------------------
	const_iterator
	begin(size_type row, size_type col) const {
		return m_[row][col];
	}
	//-----------------------------------------------------
	const_iterator
	cbegin(size_type row, size_type col) const {
		return m_[row][col];
	}

	//-----------------------------------------------------
	 iterator
	end() noexcept {
		return iterator{std::addressof(m_[nrows-1][ncols])};
	}
	const_iterator
	end() const noexcept	{
		return const_iterator{std::addressof(m_[nrows-1][ncols])};
	}
	const_iterator
	cend() const noexcept {
		return const_iterator{std::addressof(m_[nrows-1][ncols])};
	}
	//-----------------------------------------------------
	inline friend iterator
	end(this_t& i) noexcept {
		return i.end();
	}
	inline friend const_iterator
	end(const this_t& i) noexcept {
		return i.end();
	}
	inline friend const_iterator
	cend(const this_t& i) noexcept {
		return i.cend();
	}


	//---------------------------------------------------------------
	// REVERSE SEQUENTIAL ITERATORS
	//---------------------------------------------------------------
	reverse_iterator
	rbegin() noexcept {
		return reverse_iterator(end());
	}
	const_reverse_iterator
	rbegin() const noexcept {
		return const_reverse_iterator(end());
	}
	const_reverse_iterator
	crbegin() const noexcept {
		return const_reverse_iterator(end());
	}
	//-----------------------------------------------------
	inline friend reverse_iterator
	rbegin(this_t& i) noexcept {
		return i.rbegin();
	}
	inline friend const_reverse_iterator
	rbegin(const this_t& i) noexcept {
		return i.rbegin();
	}
	inline friend const_reverse_iterator
	crbegin(const this_t& i) noexcept {
		return i.crbegin();
	}

	//-----------------------------------------------------
	reverse_iterator
	rend() noexcept {
		return reverse_iterator(begin());
	}
	const_reverse_iterator
	rend() const noexcept {
		return const_reverse_iterator(begin());
	}
	const_reverse_iterator
	crend() const noexcept {
		return const_reverse_iterator(begin());
	}
	//-----------------------------------------------------
	inline friend reverse_iterator
	rend(this_t& i) noexcept {
		return i.rend();
	}
	inline friend const_reverse_iterator
	rend(const this_t& i) noexcept {
		return i.rend();
	}
	inline friend const_reverse_iterator
	crend(const this_t& i) noexcept {
		return i.crend();
	}


	//---------------------------------------------------------------
	// ROW ITERATORS
	//---------------------------------------------------------------
	row_iterator
	row_begin(size_type row) {
		return row_iterator{std::addressof(m_[row][0])};
	}
	//-----------------------------------------------------
	const_row_iterator
	row_begin(size_type row) const {
		return const_row_iterator{std::addressof(m_[row][0])};
	}
	//-----------------------------------------------------
	const_row_iterator
	row_cbegin(size_type row) const {
		return const_row_iterator{std::addressof(m_[row][0])};
	}

	//-----------------------------------------------------
	row_iterator
	row_end(size_type row) {
		return row_iterator{std::addressof(m_[row][ncols])};
	}
	//-----------------------------------------------------
	const_row_iterator
	row_end(size_type row) const {
		return const_row_iterator{std::addressof(m_[row][ncols])};
	}
	//-----------------------------------------------------
	const_row_iterator
	row_ccend(size_type row) const {
		return const_row_iterator{std::addressof(m_[row][ncols])};
	}


	//---------------------------------------------------------------
	// COLUMN ITERATORS
	//---------------------------------------------------------------
	col_iterator
	col_begin(size_type col) {
		return col_iterator{std::addressof(m_[0][col])};
	}
	//-----------------------------------------------------
	const_col_iterator
	col_begin(size_type col) const {
		return const_col_iterator{std::addressof(m_[0][col])};
	}
	//-----------------------------------------------------
	const_col_iterator
	col_cbegin(size_type col) const {
		return const_col_iterator{std::addressof(m_[0][col])};
	}

	//-----------------------------------------------------
	col_iterator
	col_end(size_type col) {
		return col_iterator{std::addressof(m_[nrows][col])};
	}
	//-----------------------------------------------------
	const_col_iterator
	col_end(size_type col) const {
		return const_col_iterator{std::addressof(m_[nrows][col])};
	}
	//-----------------------------------------------------
	const_col_iterator
	col_cend(size_type col) const {
		return const_col_iterator{std::addressof(m_[nrows][col])};
	}


	//---------------------------------------------------------------
	// DIAGONAL ITERATORS
	//---------------------------------------------------------------
	template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
	diag_iterator
	diag_begin() {
		return diag_iterator{std::addressof(m_[0][0])};
	}
	//-----------------------------------------------------
	template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
	const_diag_iterator
	diag_begin() const {
		return const_diag_iterator{std::addressof(m_[0][0])};
	}
	//-----------------------------------------------------
	template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
	const_diag_iterator
	cdiag_begin() const {
		return const_diag_iterator{std::addressof(m_[0][0])};
	}

	//-----------------------------------------------------
	template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
	diag_iterator
	diag_end() {
		return diag_iterator{std::addressof(m_[nrows][ncols])};
	}
	//-----------------------------------------------------
	template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
	const_diag_iterator
	diag_end() const {
		return const_diag_iterator{std::addressof(m_[nrows][ncols])};
	}
	//-----------------------------------------------------
	template<class T = int, class = typename std::enable_if<ncols==nrows,T>::type>
	const_diag_iterator
	cdiag_end() const {
		return const_diag_iterator{std::addressof(m_[nrows][ncols])};
	}


	//---------------------------------------------------------------
	// BLOCK ITERATORS
	//---------------------------------------------------------------
	block_iterator
	block_iter(
		size_type firstRow, size_type firstCol,
		size_type lastRow,  size_type lastCol)
	{
		difference_type stride = ncols -  lastCol - 1 + firstCol;

		return block_iterator{
			std::addressof(m_[firstRow][firstCol]),
			std::addressof(m_[lastRow][lastCol]) + stride + 1,
			static_cast<difference_type>(lastCol - firstCol + 1),
			stride };
	}
	//-----------------------------------------------------
	const_block_iterator
	block_iter(
		size_type firstRow, size_type firstCol,
		size_type lastRow,  size_type lastCol) const
	{
		difference_type stride = ncols -  lastCol - 1 + firstCol;

		return const_block_iterator{
			std::addressof(m_[firstRow][firstCol]),
			std::addressof(m_[lastRow][lastCol]) + stride + 1,
			static_cast<difference_type>(lastCol - firstCol + 1),
			stride };
	}
	//-----------------------------------------------------
	const_block_iterator
	block_citer(
		size_type firstRow, size_type firstCol,
		size_type lastRow,  size_type lastCol) const
	{
		difference_type stride = ncols -  lastCol - 1 + firstCol;

		return const_block_iterator{
			std::addressof(m_[firstRow][firstCol]),
			std::addressof(m_[lastRow][lastCol]) + stride + 1,
			static_cast<difference_type>(lastCol - firstCol + 1),
			stride };
	}


private:

	//---------------------------------------------------------------
	// DATA MEMBERS
	//---------------------------------------------------------------
	value_type m_[nrows][ncols];
};








/*****************************************************************************
 *
 *
 *
 *****************************************************************************/

//---------------------------------------------------------------
// I/O
//---------------------------------------------------------------
template<class Ostream, class T, std::size_t r, std::size_t c>
inline Ostream&
operator << (Ostream& os, const matrix<T,r,c>& m)
{
	return print(os, m);
}

//-------------------------------------------------------------------
template<class Ostream, class T, std::size_t nrows, std::size_t ncols>
inline Ostream&
print(Ostream& os, const matrix<T,nrows,ncols>& m)
{
	os << nrows <<' '<< ncols << '\n';
	if(nrows < 1 || ncols < 1) return os;

	for(std::size_t r = 0; r < nrows-1; ++r) {
		for(std::size_t c = 0; c < ncols-1; ++c) {
			os << m(r,c) << ' ';
		}
		os << m(r,ncols-1) << '\n';
	}
	for(std::size_t c = 0; c < ncols; ++c) {
		os << m(nrows-1,c) << ' ';
	}
	return os;
}


}  // namespace am


#endif
