tbb_dep = dependency('tbb', method: 'pkg-config')

platform_src = [
    'computeengine.cpp',
    'nodefactoryregistry.cpp',
    'nodestorage.cpp',
    'taskqueue.cpp'
]

platform = static_library(
    'platform',
    platform_src,
    include_directories: root_include_dir,
    dependencies: [tbb_dep]
)

platform_dep = declare_dependency(link_with: platform)
