   #  multiarray

---
```cpp
#include "multiarray.hpp"

int main() {
  cx::multiarray <int, 2, 3> m = {1, 2, 3, 4, 5, 6};

  std::cout << m; // 1 2 3 4 5 6
  
  int& min = m.min();
  int& max = m.max();
  
  std::cout << min; // 1
  std::cout << max; // 6
}
```
.....
- Multidimensional
```cpp
cx::multiarray <int, M> m_1 = {};
cx::multiarray <int, M, N> m_2 = {};
cx::multiarray <int, M, N, O> m_3 = {};
cx::multiarray <int, M, N, O, P...> m_ = {};

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
cx::multiarray <int, 2, 3> m = {};

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

m *= m;
m /= m;
m %= m;
m += m;
m -= m;
m ^= m;
m |= m;
m &= m;
m <<= m;
m >>= m;

m* m;
m / m;
m% m;
m + m;
m - m;
m^ m;
m | m;
m& m;
m << m;
m >> m;
```
```cpp
#include "multiarray.hpp"

int main() {
   cx::multiarray <int, 2, 3, 4> m =
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};

   for (std::size_t i = 0; i < 24; ++i) {
      auto subscript = m.to_subscript(i);
   
      int &x = m(subscript);
      int &y = m[i];
      int &z = m.at(subscript);
   }
   
   for (std::size_t i = 0; i < 2; ++i) {
      for (std::size_t j = 0; j < 3; ++j) {
         for (std::size_t k = 0; k < 4; ++k) {
            std::size_t linear_index = m.to_linear_index(i, j, k);
            
            int &x = m(i, j, k);
            int &y = m[linear_index];
            int &z = m.at(i, j, k);
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
cx::multiarray <int, 2, 3> m = {};

m.begin();
m.end();
m.cbegin();
m.cend();
m.rbegin();
m.rend();
m.crbegin();
m.crend();

std::copy(m.begin(), m.end(), std::ostream_iterator <int>(std::cout, " ")); // 0 0 0 0 0 0
```
.....
```cpp
cx::multiarray <std::string, 2, 3> m1 = {};
cx::multiarray <std::string, 2, 3> m2 = {};

m2.fill("X");

m1.swap(m2);

std::cout << m1; // X X X X X X
```
.....
- Customization
```cpp
#include <iostream>
#include "multiarray.hpp"

template <class t, std::size_t m, std::size_t n>
using matrix = cx::multiarray <t, m, n>;

int main() {
   matrix <int, 2, 3> m = {1, 2, 3, 4, 5, 6};
   
   for (std::size_t r = 0; r < 2; ++r) {
      for (std::size_t c = 0; c < 3; ++c) {
         std::size_t index =
             m.to_linear_index(r, c);
             
         std::cout << index << ' '; // 0 1 2 3 4 5
      }
   }
}
```
```cpp
#include <iostream>
#include "multiarray.hpp"

template <class t, std::size_t m, std::size_t n>
struct matrix : cx::multiarray <t, m, n> {
   constexpr std::size_t rsize() {
      return m;
   }
   
   constexpr std::size_t csize() {
      return n;
   }
};

int main() {
   matrix <int, 3, 2> m = {1, 2, 3, 4, 5, 6};
   
   for (std::size_t r = 0; r < m.rsize(); ++r) {
      for (std::size_t c = 0; c < m.csize(); ++c) {
         std::size_t index =
             m.to_linear_index(r, c);
             
         std::cout << index << ' '; // 0 1 2 3 4 5
      }
   }
   
   puts("\n");
   
   for (std::size_t index = 0; index < m.rsize() * m.csize(); ++index) {
      auto subscript =
          m.to_subscript(index);
          
      std::size_t r = std::get <0>(subscript);
      std::size_t c = std::get <1>(subscript);
      
      std::cout << r << ' ' << c << std::endl;
      /*
          0 0
          0 1
          1 0
          1 1
          2 0
          2 1
          
          std::cout << m(subscript) << ' '; 1 2 3 4 5 6
      */
   }
}
```
