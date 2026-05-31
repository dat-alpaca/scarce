sources = files(
    'src/loader.c',
    'src/main.c',
    'src/engine.c',
    'src/config/config.c'
)

executable('scarce', 
    sources: sources,
    dependencies: [scarce_engine_dep, inih_dep],
    install: true,
    install_tag: 'application',
    install_dir: '.'
)