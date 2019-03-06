#pragma once

#include "node.h"

#include "base/immutable/map.h"

#include <string>
#include <variant>

namespace platform
{

using MetadataProperty      = std::variant<std::string, float>;
using Metadata              = immutable::Map<std::string, MetadataProperty>;
using NodeCollection        = immutable::Map<NodeId, Node*>;
using MetadataCollection    = immutable::Map<NodeId, Metadata>;

} // namespace platform
