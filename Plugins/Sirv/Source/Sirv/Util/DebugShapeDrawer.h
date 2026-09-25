// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

class UWorld;

namespace sirv { namespace rpc { class DebugShape; }}

class FDebugShapeDrawer
{
public:

  explicit FDebugShapeDrawer(UWorld &InWorld) : World(InWorld) {}

  void Draw(const sirv::rpc::DebugShape &Shape);

private:

  UWorld &World;
};
