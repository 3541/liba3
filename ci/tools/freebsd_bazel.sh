# Build Bazel 9 for FreeBSD.
#
# Copyright (c) 2025, 2026, Alex O'Brien <3541@3541.website>
#
# This file is licensed under the BSD 3-clause license. See the LICENSE file in
# the project root for details.
#
# Workaround until binary packages are published.

#!/usr/bin/env sh

set -e

if ! -f /etc/make.conf; then
    touch /etc/make.conf
fi

if grep -qv USE_PACKAGE_DEPENDS /etc/make.conf; then
    echo 'USE_PACKAGE_DEPENDS="YES"' >> /etc/make.conf
fi

cd /usr/ports/devel/bazel9
make install -DBATCH
