sources = files(
    'src/core/fixed_array.c',
    'src/core/dynamic_array.c',
    'src/core/font/font.c',
    'src/core/random.c',
    'src/core/memory/memory.c',
    'src/core/memory/stack.c',
    'src/core/string_utils.c',
    'src/core/logging/logger.c',
    'src/core/view/view.c',
    'src/core/physics/aabb.c',
    
    'src/graphics/graphics.c',
    'src/graphics/pipeline.c',
    'src/graphics/text_renderer.c',

    'src/ui/ui.c',
    'src/ui/button.c',
    'src/ui/text_box.c',
    'src/ui/hsml/hsml.c',
    'src/ui/hsml/token.c',
    'src/ui/hsml/conditional.c',
    'src/ui/hsml/placeholder.c',
)

# Dependencies:
dependencies = [glew_dep, gl_dep, cglm_dep, freetype_dep, stb_dep]

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
        x11_dep = dependency('x11')
        sources += files('src/platform/linux/window.c')
        dependencies += x11_dep
    endif
endif

if not get_option('use_native_windowing')
    sources += files('src/platform/glfw/window.c')
    dependencies += glfw_dep
endif


# Main library:
engine_internal_includes = include_directories('src', 'src/core', 'src/graphics')
engine_includes = [engine_internal_includes]

scarce_engine_lib = static_library(
    'scarce-engine',
    sources: sources,
    include_directories: engine_includes,
    dependencies: dependencies,
)

scarce_engine_dep = declare_dependency(
    link_with: scarce_engine_lib,
    include_directories: engine_includes,
    dependencies: dependencies
)