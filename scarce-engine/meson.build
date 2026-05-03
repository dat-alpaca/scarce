sources = files(
    'src/core/fixed_array.c',
    'src/core/font/font.c',
    'src/core/random.c',
    'src/core/memory/memory.c',
    'src/core/memory/stack.c',
    'src/core/string_utils.c',

    'src/graphics/graphics.c',
    'src/graphics/pipeline.c',
    'src/graphics/text_renderer.c',
)

# Platform-dependent files:
if host_machine.system() == 'windows'
    sources += files('src/platform/win32/file.c')
    sources += files('src/platform/win32/breakpoint.c')

    if get_option('use_native_windowing')
        sources += files('src/platform/win32/window.c')
    endif

elif host_machine.system() == 'linux'
    sources += files('src/platform/linux/breakpoint.c')
    sources += files('src/platform/linux/file.c')
    sources += files('src/platform/linux/memory.c')
    sources += files('src/platform/linux/time.c')

    if get_option('use_native_windowing')
        sources += files('src/platform/linux/window.c')
    endif
endif

if not get_option('use_native_windowing')
    sources += files('src/platform/glfw/window.c')
endif


# Main library:
engine_internal_includes = include_directories('src', 'src/core', 'src/graphics')
engine_includes = [engine_internal_includes]

scarce_engine_lib = static_library(
    'scarce-engine',
    sources: sources,
    include_directories: engine_includes,
    dependencies: [glew_dep, gl_dep, cglm_dep, freetype_dep, glfw_dep, stb_dep],
)

scarce_engine_dep = declare_dependency(
    link_with: scarce_engine_lib,
    include_directories: engine_includes,
    dependencies: [glew_dep, gl_dep, cglm_dep, freetype_dep, glfw_dep, stb_dep]
)