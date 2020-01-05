   #  matrix

---
```cpp
#include "matrix.hpp"

int main() {
  cx::matrix <int, 2, 3> m = {1, 2, 3, 4, 5, 6};

  std::cout << m; // 1 2 3 4 5 6
  
  int& min = m.min();
  int& max = m.max();
  
  std::cout << min; // 1
  std::cout << max; // 6
}
```
.....
- As_array
```cpp
cx::matrix <int, 2, 3> m = {1, 2, 3, 4, 5, 6};

for (auto x : m.as_array()) {
  int &element = *x; // sanitize
  
  element *= 2;
}

std::cout << m; // 2 4 6 8 10 12
```
.....
- Multidimensional
```cpp
cx::matrix <int, 3> m_1 = {};
cx::matrix <int, 3, 3> m_2 = {};
cx::matrix <int, 3, 3, 3> m_3 = {};
cx::matrix <int, 3, 3, 3, 3...> m_ = {};

// linear index
m_1[i]
m_2[i]
m_3[i]

// subscript
m_1(i)
m_2(i, j)
m_3(i, j, k)

// index check
m_1.at(i)
m_2.at(i, j)
m_3.at(i, j, k)
```
.....
- Arithmetic
```cpp
cx::matrix <int, 2, 3> m = {};

m *= value;
m /= value;
m %= value;
m += value;
m -= value;
m ^= value;
m |= value;
m &= value;
m <<= value;
m >>= value;

m * value;
m / value;
m % value;
m + value;
m - value;
m ^ value;
m | value;
m & value;
m << value;
m >> value;

#include "matrix.hpp"
#include <cassert>

int main() {
   cx::matrix <int, 2, 3, 4> m =
   {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};

   for (std::size_t i = 0; i < 24; ++i) {
      int x = m(m.to_subscript(i));
      int y = m[i];
      int z = m.at(m.to_subscript(i));
      
      assert(x == y && x == z); // pass
   }
   
   for (std::size_t i = 0; i < 2; ++i) {
      for (std::size_t j = 0; j < 3; ++j) {
         for (std::size_t k = 0; k < 4; ++k) {
            int x = m(i, j, k);
            int y = m[m.to_linear_index(i, j, k)];
            int z = m.at(i, j, k);
            
            assert(x == y && x == z); // pass
         }
      }
   }
   
   for (std::size_t i = 0; i < 100; ++i) {
      if (m.index_out_of_bounds(m.to_subscript(i))) {
         break;
      }
      
      std::cout << m[i] << ' '; // 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
   }
}
```
.....
- Iterator
```cpp
cx::matrix <int, 2, 3> m = {};

m.begin();
m.end();
m.cbegin();
m.cend();
m.rbegin();
m.rend();
m.crbegin();
m.crend();
```
.....
```cpp
cx::matrix <std::string, 2, 3> m1 = {};
cx::matrix <std::string, 2, 3> m2 = {};

m2.fill("X");

m1.swap(m2);

std::cout << m1; // XXXXXX
```
