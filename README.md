AM containers
==========

Collection of generic containers for C++

**Note**: All classes and functions are work in progress.
Interfaces are still likely to change over time and documentation is limited to some comments so far. 

All components are header-only.


## Generic Containers

#### matrix&#95;array
  2-dimensional statically sized array

#### dynamic&#95;matrix
  2-dimensional dynamically sized array

#### triangle&#95;matrix
  lower triangle matrix (useful for storing pairwise relations)
  Note that indexing excludes the diagonal: 
  N rows => row indices: 1 ... n, column indices: 0 ... n-1

#### crs&#95;matrix
  compressed row storage (crs) sparse matrix

#### vector&#95;map
  key-&gt;value map 
  stores (key,value) pairs in one contiguous chunk of memory  
  operator[] differs from std::map: 
  it allows indexed random access to key,value pairs

#### compressed&#95;multiset
  stores a set of representatives of equivalence classes
  instead of multiple equivalent values


## Requirements
  - requires C++11 conforming compiler
  - tested with g++ 5.1.0
