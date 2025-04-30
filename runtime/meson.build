project('silk-script-rt-impl', 'c', 'java',
    license: 'EUPL',
    version: '0.1.0',
    default_options: [ 'c_std=c18', 'b_lto=true' ])

add_project_arguments(
	'-Wall',
	'-Wextra',
	'-Wno-unused-parameter',
	'-Wpedantic',
	'-Wno-missing-braces',
	language: 'c')

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)

glfw_dep = dependency('glfw3', include_type: 'system')

java_dep = dependency('jni', include_type: 'system')

thread_dep = dependency('threads', include_type: 'system')

vk_dep = dependency('vulkan', include_type: 'system')

vk_headers_dep = subproject('vulkan-headers') \
    .get_variable('vulkan_headers_dep')       \
    .as_system()

vk_validation_dep = subproject('vulkan-validationlayers')  \
    .get_variable('vulkan_validationlayers_dep')           \
    .as_system()

shared_library('ssri',
    sources: [
        'source/disas.c',
        'source/emitter.c',
        'source/stack.c',
        'source/mem.c',
        'source/object.c',
        'source/value.c',
        'source/rodata.c',
        'source/symtable.c',
        'source/stdlib.c',
        'source/garbage.c',
        'source/file.c',
        'source/env.c',
        'source/program.c',
        'source/vm.c',
        'source/runtime.c',
    ],
    cpp_args : [
        '-DGLFW_INCLUDE_VULKAN',
    ],
    install : true,
    include_directories : include_directories(
        'include',
        get_option('sbt_generated_includedir'),
    ),
    dependencies : [
        m_dep,
        glfw_dep,
        java_dep,
        thread_dep,
        vk_dep,
        vk_headers_dep,
        vk_validation_dep,
    ])
