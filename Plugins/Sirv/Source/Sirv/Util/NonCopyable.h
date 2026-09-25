// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

class NonCopyable {
public:

  NonCopyable() = default;

  NonCopyable(const NonCopyable &) = delete;

  void operator=(const NonCopyable &) = delete;
};
