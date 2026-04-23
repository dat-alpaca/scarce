sources = files(
    'src/implementation.c',
    'src/loader.c',
    'src/main.c',
    'src/config/loader.c'
)

executable('scarce', 
    sources: sources,
    dependencies: [scarce_engine_dep, inih_dep],
    install: true,
    install_tag: 'application',
    install_dir: '.'
)