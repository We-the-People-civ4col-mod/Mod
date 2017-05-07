TEMPLATE = subdirs
sub_jomlib.subdir = src/jomlib
sub_app.subdir = src/app
sub_app.depends = sub_jomlib
sub_tests.subdir = tests
sub_tests.depends = sub_jomlib sub_app
SUBDIRS = sub_app sub_jomlib sub_tests

OTHER_FILES = \
    changelog.txt
