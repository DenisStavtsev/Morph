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
};

} // namespace

void bind_node_storage()
{
    class_<NodeStorageAdaptor>("NodeStorage")
        .def("dispatch", &NodeStorageAdaptor::dispatch);

}

BOOST_PYTHON_MODULE(_platform)
{
    bind_node();
    bind_node_collection();
    bind_metadata();
    bind_metadata_collection();
}
