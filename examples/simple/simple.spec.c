#include "../../src/cSpec.h"
#include "simple.h"

module(cDataLib, {
    describe("cSpec functions", {
        before({
            debug_msg();
        });
        before_each(&initializer);

        it("succeeds `assert_that`", {
            assert_that(1 is 1);
        });
        it("fails `assert_that`", {
            assert_that(1 isnot 1);
        });

        it("succeeds `nassert_that`", {
            nassert_that(2 isnot 2);
        });
        it("fails `nassert_that`", {
            nassert_that(2 is 2);
        });

        it("succeeds `assert_that_int`", {
            assert_that_int(1 equals to 1);
        });
        it("fails `assert_that_int`", {
            assert_that_int(2 equals to 3);
        });

        it("succeeds `nassert_that_int`", {
            nassert_that_int(1 equals to 2);
        });
        it("fails `nassert_that_int`", {
            nassert_that_int(4 equals to 4);
        });

        it("succeeds `assert_that_double`", {
            assert_that_double(3.2 equals to 3.2);
        });
        it("fails `assert_that_double`", {
            assert_that_double(3.2 equals to 4.2);
        });

        xit("SKIP THAT TEST", {
            assert_that_value(1 is 1);
        });

        it("succeeds `nassert_that_double`", {
            nassert_that_double(6.6 equals to 7.7);
        });
        it("fails `nassert_that_double`", {
            nassert_that_double(6.6 equals to 6.6);
        });

        it("succeeds `assert_that_string`", {
            assert_that_string("testme" equals to "testme");
        });
        it("fails `assert_that_string`", {
            assert_that_string("testme" equals to "nottestme");
        });

        it("succeeds `nassert_that_string`", {
            nassert_that_string("testme" equals to "notthepreviousvariable");
        });
        it("fails `nassert_that_string`", {
            nassert_that_string("ok str" equals to "ok str");
        });

        after_each(&destructor);
        after({
            teardown_msg();
        });
    });
});

module(second, {
    describe("DESC 1", {
        it("before on desc 1", {
            assert_that(42 isnot 69);
        });

        describe("DESC 2", {
            describe("DESC 3", {
                it("does on 3", {
                    assert_that(3 is 3);
                });
            });
            it("does on 2", {
                assert_that(2 is 2);
            });
        });
        it("does on 1", {
            assert_that(1 is 1);
        });
    });

    describe("Array Assertions", {
        int a[5] = {1, 2, 3, 4, 5};
        int b[5] = {7, 7, 7, 7, 7};
        int c[4] = {1, 2, 3, 4};

        it("succeeds `assert_that_int_array`", {
            int my_arr[5] = {1, 2, 3, 4, 5};
            assert_that_int_array(my_arr equals to a with length 5);
        });
        it("fails `assert_that_int_array`", {
            assert_that_int_array(a equals to b with length 5);
            assert_that_int_array(b equals to c with length 5);
        });

        it("succeeds `nassert_that_int_array`", {
            nassert_that_int_array(a equals to c with length 5);
        });
        it("fails `nassert_that_int_array`", {
            int my_arr2[5] = {7, 7, 7, 7, 7};
            nassert_that_int_array(my_arr2 equals to b with length 5);
        });
    });
});

module(another_third, {
    describe("Generic type assertions", {
        context("when using generic asserts", {
            it("succeeds generic int assertion", {
                assert_that_value(1 equals to 1);
            });
            it("fails generic int assertion", {
                assert_that_value(2 equals to 3);
            });

            it("succeeds generic int nassertion", {
                nassert_that_value(1 equals to 2);
            });
            it("fails generic int nassertion", {
                nassert_that_value(4 equals to 4);
            });

            it("succeeds generic double assertion", {
                assert_that_value(3.2 equals to 3.2);
            });
            it("fails generic double assertion", {
                assert_that_value(3.2 equals to 4.2);
            });

            it("succeeds generic double nassertion", {
                nassert_that_value(6.6 equals to 7.7);
            });
            it("fails generic double nassertion", {
                nassert_that_value(6.6 equals to 6.6);
            });

            it("fails generic int with double assertion", {
                assert_that_value(2 equals to 2.2); 
            });
            it("fails generic double with int assertion", {
                assert_that_value(3.3 equals to 3);
            });

            it("succeeds generic int with double nassertion", {
                nassert_that_value(2 equals to 2.2); 
            });
            it("succeeds generic double with int nassertion", {
                nassert_that_value(3.3 equals to 3);
            });

            it("succeeds generic string assertion", {
                assert_that_value("testme" equals to "testme");
            });
            it("fails generic string assertion", {
                assert_that_value("testme" equals to "nottestme");
            });

            it("succeeds generic string nassertion", {
                nassert_that_value("testme" equals to "notthepreviousvariable");
            });
            it("fails generic string nassertion", {
                nassert_that_value("ok str" equals to "ok str");
            });
        });
        context("when we are finished", {
            it("does nothing", {
                assert_that(true);
            });
        });
    });
});

spec_suite({
    cDataLib();
    second();
    another_third();
});

spec({
    run_spec_suite("all");
    export_test_results("report.xml", "all", "xml");
});
