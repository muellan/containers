AM containers
==========

Header-only C++11/14 library of generic containers.

- [Interfaces](#interfaces)
- [Requirements](#requirements)



## Quick Overview

#### [matrix\_array](#matrix-array)
  2-dimensional statically sized array

#### [dynamic\_matrix](#dynamic-matrix)
  2-dimensional dynamically sized array

#### [triangle\_matrix](#triangle-matrix)
  lower triangle matrix that stores n\*(n-1) elements
  Note that indexing excludes the diagonal: 
  N rows => row indices: 1 ... n, column indices: 0 ... n-1

#### [crs\_matrix](#crs-sparse-matrix)
  compressed row storage (crs) sparse matrix

#### [compressed\_multiset](#compressed-multiset)
  multiset-like class that stores only one representative (of an equivalence class) per key instead of multiple equivalent values per key



## Interfaces

Note that the descriptions provided here are informal and simplified for better readability.

### common

All containers have the following member functions:
```cpp
      any_container::iterator  any_container::begin();
any_container::const_iterator  any_container::begin();
any_container::const_iterator any_container::cbegin();

      any_container::iterator  any_container::end();
any_container::const_iterator  any_container::end();
any_container::const_iterator any_container::cend();

any_container::size_type any_container::size();
bool any_container::empty();

```

The following free standing function templates are defined for all containers:
```cpp
      iterator  begin(Container&);
const_iterator  begin(const Container&);
const_iterator cbegin(const Container&);

      iterator  end(Container&);
const_iterator  end(const Container&);
const_iterator cend(const Container&);

size_type size(const Container&);
bool empty(const Container&);
```



### matrix-array
A 2-dimensional statically sized array.

```cpp
template<class ValueType, std::size_t nrows, std::size_t ncols = nrows>
class matrix_array {
public:
    class value_type;
    class size_type;

    //iterate over all elements (in row major order)
    class {const_}iterator;
    class {const_}reverse_iterator;
    
    //iterate over elements within a row
    class {const_}row_iterator;
    
    //iterate over elements within a column
    class {const_}col_iterator;
    
    //iterate over diagonal elements; only enabled if matrix is square
    class {const_}diag_iterator;
    
    //iterable range with begin(), end(), size() over elements in a row 
    class {const_}row_range;
    
    //iterable range with begin(), end(), size() over elements in a column
    class {const_}col_range;

    //iterable range with begin(), end(), size() over rectangular submatrix
    class {const_}rectangular_range;
    
    //iterable range with begin(), end(), size() over diagonal elements
    //only enabled if matrix is square
    class {const_}diag_range;

    //-----------------------------------------------------
    matrix_array() = default;
    matrix_array(std::initializer_list<std::initializer_list<value_type>>);
    matrix_array(const matrix_array&) = default;
    matrix_array(matrix_array&&) = default;
    ~matrix_array() = default;
    matrix_array& operator = (const matrix_array&) = default;
    matrix_array& operator = (matrix_array&&) = default;

    //access to single elements
    {const} value_type& operator () (size_type row, size_type col) {const} noexcept;

    //iterator range to entire row
    {const_}row_range operator [] (size_type index) {const} noexcept;

    //raw data pointer
    {const} value_type* data() {const} noexcept;

    //number of rows & columns
    static constexpr size_type rows() noexcept;
    static constexpr size_type cols() noexcept 
    static constexpr bool is_square() noexcept; 
    //total number of elements
    static constexpr size_type size() noexcept;
    static constexpr size_type max_size() noexcept;
    static constexpr bool empty() noexcept;

    //reconstruct index from pointer
    size_type row_index_of(const_iterator) const noexcept;
    size_type col_index_of(const_iterator) const noexcept;
    std::pair<size_type,size_type> index_of(const_iterator) const noexcept;

    //iterators to all elements
    {const_}iterator  {c}begin() {const} noexcept 
    {const_}iterator  {c}begin(size_type row, size_type col) {const} noexcept 
    {const_}iterator    {c}end() {const} noexcept 

    //iterators to a row / a column / the diagonal
    {const_}row_iterator {c}begin_row(size_type index) {const} noexcept 
    {const_}row_iterator   {c}end_row(size_type index) {const} noexcept 
    {const_}col_iterator {c}begin_col(size_type index) {const} noexcept 
    {const_}col_iterator   {c}end_col(size_type index) {const} noexcept 
    {const_}diag_iterator {c}begin_diag() {const} noexcept 
    {const_}diag_iterator   {c}end_diag() {const} noexcept 

    //iterator ranges to a row, a column, the diagonal
    {const_}row_range {c}row(size_type index) {const} noexcept 
    {const_}col_range {c}col(size_type index) {const} noexcept 
    {const_}diag_range {c}diag() {const} noexcept 

    {const_}rectangular_range rectangle(size_type fstRow, size_type fstCol, 
                                        size_type lstRow, size_type lstCol) noexcept;
};
```



### dynamic matrix
A 2-dimensional dynamically sized, allocator-aware array.

```cpp
template<class ValueType, class Allocator>
class dynamic_matrix {
public:
    class value_type;
    class size_type;

    //iterate over all elements (in row major order)
    class {const_}iterator;
    class {const_}reverse_iterator;
    
    //iterate over elements within a row
    class {const_}row_iterator;
    
    //iterate over elements within a column
    class {const_}col_iterator;
    
    //iterable range with begin(), end(), size() over elements in a row 
    class {const_}row_range;
    
    //iterable range with begin(), end(), size() over elements in a column
    class {const_}col_range;

    //iterable range with begin(), end(), size() over rectangular submatrix
    class {const_}rectangular_range;

    //-----------------------------------------------------
    dynamic_matrix();
    //initialize as 1 row
    dynamic_matrix(std::initializer_list<value_type>);
    //throws execption if row dimensions are not all the same
    dynamic_matrix(std::initializer_list<std::initializer_list<value_type>>);
    dynamic_matrix(const dynamic_matrix&);
    dynamic_matrix(dynamic_matrix&&);
    ~dynamic_matrix();
    dynamic_matrix& operator = (const dynamic_matrix&);
    dynamic_matrix& operator = (dynamic_matrix&&);

    //access to single elements
    {const} value_type& operator () (size_type row, size_type col) {const} noexcept;

    //iterator range to entire row
    {const_}row_range operator [] (size_type index) {const} noexcept;

    //raw data pointer
    {const} value_type* data() {const} noexcept;

    //number of rows & columns
    size_type rows() const noexcept;
    void rows(size_type n);
    void rows(size_type n, const value_type&);

    size_type cols() const noexcept;
    void cols(size_type n);
    void cols(size_type n, const value_type&);

    void reserve(size_type nrows, size_type ncols);
    void resize(size_type nrows, size_type ncols);
    void resize(size_type nrows, size_type ncols, const value_type&);

    //total number of elements
    size_type size() const noexcept;
    size_type capacity() const noexcept;
    static constexpr size_type max_size() noexcept;
    bool is_square() const noexcept; 
    bool empty() const noexcept;

    //insert single row or column
    col_iterator insert_col(size_type index);
    col_iterator insert_col(size_type index, const value_type&);
    row_iterator insert_row(size_type index) 
    row_iterator insert_row(size_type index, const value_type&);

    //insert multiple rows or columns
    col_iterator insert_cols(size_type index, size_type quantity)
    col_iterator insert_cols(size_type index, size_type quantity, const value_type&)
    row_iterator insert_rows(size_type index, size_type quantity);
    row_iterator insert_rows(size_type index, size_type quantity, const value_type&);
 
    //erase rows / columns
    void erase_col(size_type index);
    void erase_row(size_type index);
    void erase_cols(size_type first, size_type last);
    void erase_rows(size_type first, size_type last);
    void clear();

    //reconstruct index from pointer
    size_type row_index_of(const_iterator) const noexcept;
    size_type col_index_of(const_iterator) const noexcept;
    std::pair<size_type,size_type> index_of(const_iterator) const noexcept;

    //iterators to all elements
    {const_}iterator  {c}begin() {const} noexcept 
    {const_}iterator  {c}begin(size_type row, size_type col) {const} noexcept 
    {const_}iterator    {c}end() {const} noexcept 

    //iterators to a row / a column
    {const_}row_iterator {c}begin_row(size_type index) {const} noexcept 
    {const_}row_iterator   {c}end_row(size_type index) {const} noexcept 
    {const_}col_iterator {c}begin_col(size_type index) {const} noexcept 
    {const_}col_iterator   {c}end_col(size_type index) {const} noexcept 

    //iterator ranges to a row / a column
    {const_}row_range {c}row(size_type index) {const} noexcept 
    {const_}col_range {c}col(size_type index) {const} noexcept 

    {const_}rectangular_range rectangle(size_type fstRow, size_type fstCol, 
                                        size_type lstRow, size_type lstCol) noexcept;

    const allocator_type& get_allocator() const;
};
```



### triangle matrix
A lower triangle matrix that stores n\*(n-1) elements. The size can be changed at runtime and the container is allocator-aware.

Note that indexing excludes the diagonal: N rows => row indices: 1 ... n, column indices: 0 ... n-1.

One special mechanism provided is the iteration over all elements that share the same index either as row or as column index. Suppose you have the triangle matrix
```
    |      column
row |  0    1    2    3
----+------------------
 1  | 10
 2  | 20   21
 3  | 30   31   32
 4  | 40   41   42   43
```
then all elements that 'share index 2' are: 20, 21, 32, 42
This is especially useful if the matrix cells represent values of a pairwise symmetric relation, like e.g. forces between physical objects. 


```cpp
template<class ValueType, class Allocator>
class triangle_matrix {
public:
    class value_type;
    class size_type;

    //iterate over all elements (in row major order)
    class {const_}iterator;
    class {const_}reverse_iterator;
    
    //iterate over elements within a row
    class {const_}row_iterator;
    
    //iterate over elements within a column
    class {const_}col_iterator;
    
    //iterate over all elements that have the same index as column or row
    class {const_}index_iterator;
    
    //iterable range with begin(), end(), size() over elements in a row 
    class {const_}row_range;
    
    //iterable range with begin(), end(), size() over elements in a column
    class {const_}col_range;

    //iterable range with begin(), end(), size() over elements with same index
    class {const_}index_range;

    //iterable range with begin(), end(), size() over multiple elements 
    class {const_}range;

    //-----------------------------------------------------
    triangle_matrix();
    //initialize as 1 row
    triangle_matrix(const triangle_matrix&);
    triangle_matrix(triangle_matrix&&);
    ~triangle_matrix();
    triangle_matrix& operator = (const triangle_matrix&);
    triangle_matrix& operator = (triangle_matrix&&);

    //access to single elements
    {const} value_type& operator () (size_type row, size_type col) {const} noexcept;

    //iterator range to entire row
    {const_}row_range operator [] (size_type index) {const} noexcept;

    //raw data pointer
    {const} value_type* data() {const} noexcept;

    //number of rows & columns
    size_type cols() const noexcept;
    size_type rows() const noexcept;
    void rows(size_type n);
    void rows(size_type n, const value_type&);

    void reserve_rows(size_type nrows);

    //total number of elements
    size_type size() const noexcept;
    size_type capacity() const noexcept;
    static constexpr size_type max_size() noexcept;
    bool empty() const noexcept;

    //insert elements before the elements with 'index' as row or column
    col_iterator insert_at(size_type index);
    col_iterator insert_at(size_type index, const value_type&);
    col_iterator insert_at(size_type index, size_type quantity, const value_type&);

    //erase all elements with the same index as row or column
    void erase_at(size_type index);
    void erase_at(size_type firstIndex, size_type lastIndex);

    //erase all elements with the largest index as row or column
    void erase_last();

    //erase all elements
    void clear();

    //reconstruct index from pointer
    std::pair<size_type,size_type> index_of(const_iterator) const noexcept;

    //iterators to all elements
    {const_}iterator  {c}begin() {const} noexcept 
    {const_}iterator    {c}end() {const} noexcept 

    //iterators to a row / a column / the diagonal
    {const_}row_iterator {c}begin_row(size_type index) {const} noexcept 
    {const_}row_iterator   {c}end_row(size_type index) {const} noexcept 
    {const_}col_iterator {c}begin_col(size_type index) {const} noexcept 
    {const_}col_iterator   {c}end_col(size_type index) {const} noexcept 

    //iterators to all elements that share the same index as row or column
    {const_}index_iterator {c}begin_at(size_type index) {const} noexcept 
    {const_}index_iterator   {c}end_at(size_type index) {const} noexcept 

    //iterators to a row / a column
    {const_}row_iterator {c}begin_row(size_type index) {const} noexcept 
    {const_}row_iterator   {c}end_row(size_type index) {const} noexcept 
    {const_}col_iterator {c}begin_col(size_type index) {const} noexcept 
    {const_}col_iterator   {c}end_col(size_type index) {const} noexcept 

    //iterator range to all elements that share the same index as row or column
    {const_}index_range index_interval(size_type index);
    {const_}range index_interval(size_type firstIndex, size_type lastIndex);

    const allocator_type& get_allocator() const;
};

```



### crs sparse matrix
A compressed row storage (crs) sparse matrix.

```cpp
```



### compressed multiset
A multiset-like class that stores only one representative (of an equivalence class) per key instead of multiple equivalent values per key.

```cpp
```



## Requirements
  - requires C++11 conforming compiler
  - tested with g++ 5.1.0


