// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.



#include "RandomEngine.h"
#include "Sirv.h"

#include <limits>

uint64 URandomEngine::GenerateRandomId()
{
  // Tuned to be fast as this function might get called relatively often.
  static thread_local std::mt19937_64 Engine((std::random_device())());
  std::uniform_int_distribution<uint64> Distribution(
      std::numeric_limits<uint64>::lowest(),
      std::numeric_limits<uint64>::max());
  return Distribution(Engine);
}

int32 URandomEngine::GenerateRandomSeed()
{
  std::random_device RandomDevice;
  std::uniform_int_distribution<int32> Distribution(
      std::numeric_limits<int32>::lowest(),
      std::numeric_limits<int32>::max());
  return Distribution(RandomDevice);
}

int32 URandomEngine::GenerateSeed()
{
  return GetUniformIntInRange(
      std::numeric_limits<int32>::lowest(),
      std::numeric_limits<int32>::max());
}
