// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

template<typename IT>
class ListView
{
public:

  using iterator = IT;

  explicit ListView(iterator begin, iterator end) : Begin(begin), End(end) {}

  template <typename STL_CONTAINER>
  explicit ListView(STL_CONTAINER &StlContainer) :
    Begin(iterator(StlContainer.begin())),
    End(iterator(StlContainer.end())) {}

  ListView(const ListView &) = default;
  ListView &operator=(const ListView &) = delete;

  iterator begin() const {
    return Begin;
  }

  iterator end() const {
    return End;
  }

  bool empty() const {
    return Begin == End;
  }

private:

  const iterator Begin;

  const iterator End;
};
