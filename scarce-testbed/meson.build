sources = files(
    'src/main.c',
    'src/renderer.c',
    'src/timing.c',
    'src/utils.c',
)

executable(
    'scarce-testbed',
    sources: sources,
    dependencies: [application_dep],
    c_args: application_c_args,
    link_args: application_link_args,
    link_depends: linker_script,
    pie: true,
    gnu_symbol_visibility: 'hidden',

    dependencies: [cglm_includes, freetype_includes],
    include_directories: [engine_internal_includes],

    install: true,
    install_dir: '.',
    install_tag: 'application'
)

meson.add_install_script(
    post_install,
    'scarce-testbed',
    'scarce-testbed.bin',
    install_tag: 'application'
)