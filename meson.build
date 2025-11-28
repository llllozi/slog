project(
    'slog',
    'c',
    meson_version: '>= 1.3.0',
    version: '0.1',
    default_options: ['warning_level=3'],
)

inc = include_directories('.')

cunit = dependency('cunit', required: true)

example = executable('example', 'example.c')

subdir('tests')

