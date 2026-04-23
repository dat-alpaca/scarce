sources = files(
    'src/main.c',
    'src/renderer.c',
    'src/timing.c',
    'src/utils.c',
)

testbed = executable(
    'scarce-testbed',
    sources: sources,

    # PIC:
    pie: true,
    c_args: [
        '-fvisibility=hidden',
        '-ffreestanding',
        '-fno-stack-protector',
        '-fno-asynchronous-unwind-tables',
        '-fno-ident',
        '-Os'
    ],
    gnu_symbol_visibility: 'hidden',

    # Linker:
    link_args: [
        '-nostdlib',
        '-static',
        '-fno-common',
        '-fno-stack-protector',
        '-s',
        '-T' + linker_script,
        '-Wl,--build-id=none',
        '-Wl,-n',
    ],
    link_depends: linker_script,

    dependencies: [cglm_includes, freetype_includes],
    include_directories: [engine_internal_includes],

    install: true,
    install_dir: '.',
    install_tag: 'application'
)

meson.add_install_script(
    meson.project_source_root() / 'scripts/post_install.py',
    'scarce-testbed',
    'scarce-testbed.bin',
    install_tag: 'application'
)