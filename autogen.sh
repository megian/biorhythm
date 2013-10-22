#!/bin/bash
autoreconf --force --install || exit 1
intltoolize --copy --force --automake || exit 1
