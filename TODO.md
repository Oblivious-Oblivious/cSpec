* Complete exporting to markdown and html (lines 1244, 1251)
* Fix error logging of array with less elements than expected
    e.g:   [1,2,3,4, ] expected but got [1,2,3,4,5]
* Fix on macos clang, makes a fork bomb ->
    export_test_results("report.xml", "all", "xml");
