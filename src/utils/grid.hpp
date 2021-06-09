#ifndef UTIL_GRID_HPP
#define UTIL_GRID_HPP

#include <vector>
#include <cassert>
#include <utility>
#include "core/common.hpp"
//Copied from Entt pacman
template <typename Elem>
class Grid {
public:
  Grid()
    : size{0, 0}, storage{} {}

  explicit Grid(uvec2 size)
    : size{size}, storage(area()) {}

  int area() const {
    return size.x * size.y;
  }

  int width() const {
    return size.x;
  }

  int height() const {
    return size.y;
  }

  bool outOfRange(const std::size_t i) const {
    return i >= area();
  }

  bool outOfRange(const vec2  ivec2) const {
    return  ivec2.x < 0 ||  ivec2.y < 0 ||
            ivec2.x >= size.x ||  ivec2.y >= size.y;
  }

  Elem &operator[](const std::size_t i) {
    return const_cast<Elem &>(std::as_const(*this)[i]);
  }

  Elem &operator[](const  ivec2  ivec2) {
    return const_cast<Elem &>(std::as_const(*this)[ ivec2]);
  }

  const Elem &operator[](const std::size_t i) const {
    assert(!outOfRange(i));
    return storage[i];
  }

  const Elem &operator[](const  ivec2  ivec2) const {
    assert(!outOfRange( ivec2));
    return storage[ ivec2.y * size.x +  ivec2.x];
  }

  void reset(Elem elem){
      for(int x = 0; x < size.x; x++){
          for(int y = 0; y<size.y; y++){
              storage[y * size.x +  x]=elem;
          }
      }
  }

private:
   ivec2 size;
   std::vector<Elem> storage;
};

#endif
