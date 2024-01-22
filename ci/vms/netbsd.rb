Vagrant.configure("2") do |config|
  config.vm.box = "generic/netbsd9"
  config.vm.define "netbsd"
  config.vm.synced_folder ".", "/build", type: "rsync", rsync__exclude: [".git", ".direnv", "compile_commands.json"]
  config.vm.provision "shell", inline: "pkgin -y install meson git mozilla-rootcerts-openssl"
end
