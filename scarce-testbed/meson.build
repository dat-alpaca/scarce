sources = files(
    'src/main.c',
    'src/view.c',
)

executable(
    'scarce-testbed',
    sources: sources,
    include_directories: [engine_internal_includes],

    dependencies: application_dep,
    c_args: application_c_args,
    link_args: application_link_args,
    link_depends: linker_script,
    pie: true,
    gnu_symbol_visibility: 'hidden',

    dependencies: [executable_dependencies],

    install: true,
    install_dir: '.',
    install_tag: 'application'
)

meson.add_install_script(
    post_install,
    'scarce-testbed',
    'main.bin',
    install_tag: 'application'
)