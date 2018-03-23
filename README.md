AM containers
==========

Collection of generic containers for C++14. 

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




## Requirements
  - requires C++14 conforming compiler
  - tested with g++ 6.1.0, 7.2 and clang++ 5.0.1

