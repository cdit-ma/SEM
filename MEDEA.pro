TEMPLATE = subdirs
CONFIG += ordered


SUBDIRS = \
        src \
        app \
        test

# Both app/test depends on source
app.depends = src
test.depends = src

