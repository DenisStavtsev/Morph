#pragma once

#include "knob.h"

#include "base/immutable/map.h"

#include <cstdint>
#include <string>


namespace platform
{

using NodeId = std::uint64_t;
using KnobId = std::uint64_t;

class Node
{
  public:
    virtual void compute() const = 0;

    virtual ~Node() = default;
};

} // namespace platform
