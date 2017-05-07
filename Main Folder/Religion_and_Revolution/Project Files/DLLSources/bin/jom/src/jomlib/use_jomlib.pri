isEmpty(PROJECT_BUILD_ROOT):error(PROJECT_BUILD_ROOT must be set)

build_pass:CONFIG(debug, debug|release) {
    JOMLIB = $$PROJECT_BUILD_ROOT/lib/jomlibd.lib
}
build_pass:CONFIG(release, debug|release) {
    JOMLIB = $$PROJECT_BUILD_ROOT/lib/jomlib.lib
}
LIBS += $$JOMLIB
POST_TARGETDEPS += $$JOMLIB
unset(JOMLIB)
