#!/bin/bash
xgettext -k_ -kN_ --keyword=Q_:1g -o biorhythm.pot ../src/*.c
msgmerge -o de.po de.po biorhythm.pot
