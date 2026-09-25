// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include <deque>
#include <memory>

/// A stack to keep track of nested scopes.
template <typename T>
class FScopedStack : private std::deque<T> {
  using Super = std::deque<T>;
public:

  /// Push this scope into the stack. Automatically pops @a Value when the
  /// returned object goes out of the scope.
  template <typename V>
  auto PushScope(V &&Value)
  {
    Super::emplace_back(std::forward<V>(Value));
    T *Pointer = &Super::back();
    auto Deleter = [this](const T *) { Super::pop_back(); };
    return std::unique_ptr<T, decltype(Deleter)>(Pointer, Deleter);
  }

  using Super::empty;
  using Super::size;
  using Super::begin;
  using Super::end;
};
