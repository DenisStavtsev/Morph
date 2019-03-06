#include "nodestorage.h"

#include "node.h"
#include "nodefactory.h"
#include "nodefactoryregistry.h"
#include "nodestorageactions.h"
#include "nodestoragetypes.h"
#include "taskqueue.h"

#include <vector>

namespace
{

using namespace platform;

class Reducer
{
  public:
    Reducer(NodeFactoryRegistry* registry)
        : m_node_factory_registry(registry)
    {}

    NodeStorage::State reduce(NodeStorage::State state, NodeStorageAction&& action)
    {
        // TODO: handle exceptions.
        return std::visit(
            [&](auto&& a)
            { 
                return reduce(std::move(state), std::move(a)); 
            },
            std::move(action));
    }

    NodeStorage::State reduce(NodeStorage::State&& state, CreateNode&& action)
    {
        auto factory = m_node_factory_registry->get_node_factory(action.model);
        Node* n = factory->create();

        auto next_node_state = state.m_nodes.set(action.id, n);
        auto next_metadata_state = state.m_metadata.set(action.id, std::move(action.metadata));

        return {std::move(next_node_state), std::move(next_metadata_state)};
    }

    NodeStorage::State reduce(NodeStorage::State&& state, RemoveNode&& action)
    {
        // TODO.
        return state;
    }

    NodeStorage::State reduce(NodeStorage::State&& state, UpdateMetadata&& action)
    {
        auto next_metadata_state = std::move(state.m_metadata);
        auto metadata = next_metadata_state.get(action.id);

        for (auto pair : action.metadata)
        {
            metadata = metadata.set(pair.first, pair.second);
        }

        next_metadata_state = next_metadata_state.set(action.id, metadata);

        return {std::move(state.m_nodes), std::move(next_metadata_state)};
    }

  private:
    NodeFactoryRegistry* m_node_factory_registry;
};

} // namespace

namespace platform
{

struct NodeStorage::Impl
{
    Impl(NodeFactoryRegistry* registry)
        : m_reducer(registry)
    {}

    TaskQueue               m_action_queue;
    State                   m_state;
    Reducer                 m_reducer;
    std::vector<OnUpdateFn> m_subscribers;
};

NodeStorage::NodeStorage(NodeFactoryRegistry* registry)
    : impl(new Impl(registry))
{}

NodeStorage::~NodeStorage()
{
    delete impl;
}

void NodeStorage::dispatch(NodeStorageAction action)
{
    impl->m_action_queue.post([=]() mutable 
    {
        impl->m_state = impl->m_reducer.reduce(
            std::move(impl->m_state),
            std::move(action));
        
        for (auto& cb : impl->m_subscribers)
        {
            cb();
        }
    });
}

NodeStorage::State NodeStorage::state() const
{
    // TODO: synchronization needed! (atomic root ptr in the map?)
    return impl->m_state;
}

void NodeStorage::subscribe(OnUpdateFn on_update)
{
    impl->m_subscribers.push_back(std::move(on_update));
}

} // namespace platform
