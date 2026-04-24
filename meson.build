project('scarce', 'c',
        version: '0.1',
        default_options : [
                'prefix=' + meson.current_source_dir() / 'deploy',                
                'c_std=c11',
        ])

# Dependencies:
gl_dep          = dependency('gl')
cglm_dep        = dependency('cglm', static: true)
freetype_dep    = dependency('freetype2', static: true)
glfw_dep        = dependency('glfw3', static: true)
glew_dep        = dependency('glew', static: true, fallback: ['glew', 'glew_dep'])
inih_dep        = dependency('inih', static: true)
stb_dep         = dependency('stb')

cglm_includes     = cglm_dep.partial_dependency(compile_args: true, includes: true)
freetype_includes = freetype_dep.partial_dependency(compile_args: true, includes: true)

# Paths:
linker_script = meson.current_source_dir() / 'linker.ld'
post_install = meson.current_source_dir() / 'scripts/post_install.py'

# Subdirectories:
subdir('scarce-engine')

# Application:
application_c_args = [
    '-fvisibility=hidden', 
    '-ffreestanding', 
    '-fno-stack-protector',
    '-fno-asynchronous-unwind-tables', 
    '-fno-ident', 
    '-Os'
]
application_link_args = [
    '-nostdlib', 
    '-static', 
    '-fno-common', 
    '-fno-stack-protector', 
    '-s',
    '-T' + linker_script,
    '-Wl,--build-id=none', 
    '-Wl,-n',
]

application_dep = declare_dependency(
    include_directories: engine_internal_includes,
    dependencies: [cglm_includes, freetype_includes],
    link_args: [application_link_args],
)

if get_option('build_loader')
    subdir('scarce-loader')

    install_subdir('assets/core',
        install_dir: 'assets',
        install_tag: 'application',
        strip_directory: false
    )
endif

if get_option('build_testbed')
    subdir('scarce-testbed')
endif