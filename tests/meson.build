test_names = ['test_memory', 'test_nodes', 'test_output']
slog_dep = declare_dependency(include_directories: inc)
foreach t_name : test_names
    t_exe = executable(
        t_name,
        t_name + '.c',
        dependencies: [cunit, slog_dep],
        install: false,
    )
    test(t_name, t_exe)
endforeach
