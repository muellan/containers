AM containers
==========

Collection of generic containers for C++

Note: 
All classes and functions are work in progress. Interfaces are still likely 
to change over time and documentation is limited to some comments so far. 

All components are header-only.


Generic Containers
- matrix_array</br>
<p>
  2-dimensional statically sized array
</p>
- dynamic_matrix</br>
<p>
  2-dimensional dynamically sized array
</p>
- triangle_matrix</br>
<p>
  lower triangle matrix (useful for storing pairwise relations)
  Note that indexing excludes the diagonal: 
  N rows => row indices: 1 ... n, column indices: 0 ... n-1
</p>
- crs_matrix</br>
<p>
  compressed row storage (crs) sparse matrix
</p>
- vector_map</br>
<p>
  key->value map 
  stores (key,value) pairs in one contiguous chunk of memory  
  operator[] differs from std::map: 
  it allows indexed random access to key,value pairs
</p>

Prerequisites
  - requires C++11 conforming compiler
  - tested with g++ 5.1.0
