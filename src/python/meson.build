boost_dep = dependency('boost', modules: ['python3'])
python_dep = dependency('python3')

bindplatform_src = [
    'platform/module.cpp',
]

bindplatform = shared_library(
    '_platform',
    bindplatform_src,
    include_directories: root_include_dir,
    dependencies: [platform_dep, boost_dep, python_dep],
    name_prefix: ''
)