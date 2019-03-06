#include "platform/node.h"
#include "platform/nodestorage.h"
#include "platform/nodestoragetypes.h"

#include <boost/python.hpp>

using namespace boost::python;
using namespace platform;

void bind_node()
{
    class_<Node, boost::noncopyable>("Node", no_init);
}

void bind_node_collection()
{
    class_<NodeCollection>("NodeCollection")
        .def(
            "__getitem__",
            &NodeCollection::get,
            boost::python::return_value_policy<copy_const_reference>());
}

namespace
{

struct MetadataPropertyConverter
{
    static PyObject* convert(const MetadataProperty& property)
    {
        return std::visit(
            [](const auto& value)
            {
                return convert(value); 
            }, property);
    }

    static PyObject* convert(const std::string& value)
    {
        return PyUnicode_FromString(value.c_str());
    }

    static PyObject* convert(float value)
    {
        return PyFloat_FromDouble(value);
    }
};

} // namespace

void bind_metadata()
{
    to_python_converter<MetadataProperty, MetadataPropertyConverter>();

    class_<Metadata>("Metadata")
        .def(
            "__getitem__",
            &Metadata::get,
            boost::python::return_value_policy<copy_const_reference>());
}

void bind_metadata_collection()
{
    class_<MetadataCollection>("MetadataCollection")
        .def(
            "__getitem__",
            &MetadataCollection::get,
            boost::python::return_value_policy<copy_const_reference>());
}

namespace
{

class NodeStorageAdaptor : private NodeStorage
{
  public:
    using NodeStorage::State;
    using NodeStorage::state;  
    
    NodeStorageAdaptor()
        : NodeStorage(nullptr)
    {}

    void dispatch(const dict& action)
    {
        std::string action_type = extract<std::string>(action["type"]);

        if (action_type == "CreateNode")
        {
            CreateNode create_node;
            create_node.id = extract<NodeId>(action["id"]);
            create_node.model = extract<std::string>(action["model"]);

            NodeStorage::dispatch(std::move(create_node));
        }
        else if (action_type == "RemoveNode")
        {
            RemoveNode remove_node;
            remove_node.id = extract<NodeId>(action["id"]);

            NodeStorage::dispatch(std::move(remove_node));
        }
        else if (action_type == "UpdateMetadata")
        {
            UpdateMetadata update_metadata;
            update_metadata.id = extract<NodeId>(action["id"]);

            NodeStorage::dispatch(update_metadata);
        }

        // TODO: error: unsupported action type.
    }

    void subscribe(const object& on_update)
    {
        NodeStorage::subscribe(
            [on_update]()
            {
                on_update();
            });
    }
};

} // namespace

void bind_node_storage()
{
    class_<NodeStorage::State>("NodeStorageState")
        .add_property(
            "nodes",
            +[](const NodeStorage::State& state)
            {
                return state.m_nodes;
            })
        .add_property(
            "metadata",
            +[](const NodeStorage::State& state)
            {
                return state.m_metadata;
            });

    class_<NodeStorageAdaptor>("NodeStorage")
        .def("dispatch", &NodeStorageAdaptor::dispatch)
        .def("state", &NodeStorageAdaptor::state)
        .def("subscribe", &NodeStorageAdaptor::subscribe);

}

BOOST_PYTHON_MODULE(_platform)
{
    bind_node();
    bind_node_collection();
    bind_metadata();
    bind_metadata_collection();
    bind_node_storage();
}
