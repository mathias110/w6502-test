fips_begin_app(w65c02-tests cmdline)
    fips_vs_warning_level(3)
    fips_files(
        main.c
        real_w65c02.c
        utest.h
        w65c02.c
        w65c02_test.c 
        w65c02_real.c
        w65c02dasm_test.c 
    )
fips_end_app()

