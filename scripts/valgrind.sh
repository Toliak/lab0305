#! /bin/sh

./scripts/tests.sh

set -ex
cd .build/
valgrind ./tests
