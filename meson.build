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
stb_inc         = include_directories('subprojects/stb')

cglm_includes     = cglm_dep.partial_dependency(compile_args: true, includes: true)
freetype_includes = freetype_dep.partial_dependency(compile_args: true, includes: true)

# Linker:
linker_script = meson.current_source_dir() / 'linker.ld'

# Subdirectories:
subdir('scarce-engine')

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