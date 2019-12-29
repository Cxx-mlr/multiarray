# matrix

-
- Basic usage
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
- As_array
```cpp
cx::matrix <int, 2, 3> m = {1, 2, 3, 4, 5, 6};

for (auto x : m.as_array()) {
  int &element = *x; // sanitize
  
  element *= 2;
}

std::cout << m; // 2 4 6 8 10 12
```
- Multidimensional
```cpp
cx::matrix <int, 3> m_1 = {};
cx::matrix <int, 3, 3> m_2 = {};
cx::matrix <int, 3, 3, 3> m_3 = {};
cx::matrix <int, 3, 3, 3, 3...> m_ = {};

m_1[index]
m_2[index][index]
m_3[index][index][index]

m_1(index)
m_2(index, index)
m_3(index, index, index)

m_1.at(index)
m_2.at(index, index)
m_3.at(index, index, index)
```
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
```
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
-
```cpp
cx::matrix <std::string, 2, 3> m1 = {};
cx::matrix <std::string, 2, 3> m2 = {};

m2.fill(5);

m1.swap(m2);
```
-
-
----Comming soon

more matrix computation support
