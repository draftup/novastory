do_test(Logic_Users test-LogicUsers.cpp)
do_test(Recaptcha test-Recaptcha.cpp)
do_test(JsonThrower test-JsonThrower.cpp)
do_test(TextEditor test-TextEditor.cpp)
do_test(TextRevision test-TextRevision.cpp)
do_test(Avatars test-Avatars.cpp)
append_test_resource(Avatars ${CMAKE_SOURCE_DIR}/public/images/404.jpg)
do_test(BookInfo test-BookInfo.cpp)

