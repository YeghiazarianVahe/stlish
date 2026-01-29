add_test([=[Smoke.Basic]=]  [==[C:/Users/Vahe Yeghiazaryan/Documents/dev/Portfolio/stlish/build/tests/stlish_smoke_test.exe]==] [==[--gtest_filter=Smoke.Basic]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Smoke.Basic]=]  PROPERTIES DEF_SOURCE_LINE [==[C:/Users/Vahe Yeghiazaryan/Documents/dev/Portfolio/stlish/tests/test_smoke.cpp:3]==] WORKING_DIRECTORY [==[C:/Users/Vahe Yeghiazaryan/Documents/dev/Portfolio/stlish/build/tests]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  stlish_smoke_test_TESTS Smoke.Basic)
