GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/califa/simulation/runsim.C)
add_test(califasim2 ${R3BROOT_BINARY_DIR}/macros/r3b/califa/simulation/runsim.sh)
SET_TESTS_PROPERTIES(califasim2 PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(califasim2 PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully.")
