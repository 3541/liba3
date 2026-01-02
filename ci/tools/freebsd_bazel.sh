# Build Bazel 8 for FreeBSD.
#
# Copyright (c) 2025, Alex O'Brien <3541@3541.website>
#
# This file is licensed under the BSD 3-clause license. See the LICENSE file in
# the project root for details.
#
# Workaround until the port is added: https://bugs.freebsd.org/bugzilla/show_bug.cgi?id=287546

#!/usr/bin/env sh

set -e

if [ ! -d /usr/ports/devel/bazel8 ]; then
    patch_file="$(mktemp)"
    curl https://bz-attachments.freebsd.org/attachment.cgi?id=265221 > "$patch_file"

    cd /usr/ports
    git apply "$patch_file"
    rm "$patch_file"
fi

echo 'USE_PACKAGE_DEPENDS="YES"' >> /etc/make.conf

cd /usr/ports/devel/bazel8
make install -DBATCH
