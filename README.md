AM containers
==========

Collection of generic containers for C++

Note: 
All classes and functions are work in progress. Interfaces are still likely 
to change over time and documentation is very poor so far. 

All components are header-only.


Generic Containers
- matrix</br>
<p>
  2-dimensional statically sized array
</p>
- dynmatrix</br>
<p>
  2-dimensional dynamically sized array
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
  - tested with g++ 4.7.2
