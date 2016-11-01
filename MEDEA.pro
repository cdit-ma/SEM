TEMPLATE = subdirs
CONFIG += ordered


SUBDIRS = \
        src \
        test \
        app

# Both app/test depends on source
app.depends = src
test.depends = src
