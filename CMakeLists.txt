#
# project: w65c02
#
cmake_minimum_required(VERSION 2.8)
project(w65c02)
# include the fips main cmake file
get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include("${FIPS_ROOT_DIR}/cmake/fips.cmake")

# include_directories(src)
fips_setup()
fips_add_subdirectory(test)
fips_finish()


