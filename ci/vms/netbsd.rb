Vagrant.configure("2") do |config|
  config.vm.box = "generic/netbsd9"
  config.vm.define "netbsd"
  config.vm.synced_folder ".", "/build", type: "rsync", rsync__exclude: [".git", ".direnv", "compile_commands.json"]
  config.vm.provision "shell", inline: <<-EOF
    pkgin -y install git mozilla-rootcerts-openssl python312 py312-pip ninja-build gcc8
    ln -sf /usr/pkg/gcc8/bin/cc /usr/bin/cc
    ln -sf /usr/pkg/gcc8/bin/c++ /usr/bin/c++
    pip install meson
  EOF
end
