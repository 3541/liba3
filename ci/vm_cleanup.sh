#!/usr/bin/env bash

set -euo pipefail

cd "$(git rev-parse --show-toplevel)"

for vm in ci/vms/*.rb; do
    VAGRANT_VAGRANTFILE="$vm" vagrant destroy
done
