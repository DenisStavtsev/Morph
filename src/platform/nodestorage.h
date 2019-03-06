#pragma once

#include "nodestorageactions.h"
#include "nodestoragetypes.h"

#include "base/immutable/map.h"

#include <functional>


namespace platform { class Node; }
namespace platform { class NodeContext; }
namespace platform { class NodeFactoryRegistry; }

namespace platform
{

class NodeStorage
{
  public:
    struct State
    {
        NodeCollection      m_nodes;
        MetadataCollection  m_metadata;
    };

    using OnUpdateFn = std::function<void()>;

    NodeStorage(NodeFactoryRegistry* registry);
    ~NodeStorage();

    void    dispatch(NodeStorageAction action);
    State   state() const;
    void    subscribe(OnUpdateFn on_update);

  private:
    struct Impl;
    Impl* impl;
};

} // namespace platform
