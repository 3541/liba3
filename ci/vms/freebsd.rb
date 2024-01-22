Vagrant.configure("2") do |config|
  config.vm.box = "generic/freebsd13"
  config.vm.define "freebsd"
  config.vm.synced_folder ".", "/build", type: "rsync", rsync__exclude: [".git", ".direnv", "compile_commands.json"]
  config.vm.provision "shell", inline: "pkg install -y meson git"
end
