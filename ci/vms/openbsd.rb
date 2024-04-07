Vagrant.configure("2") do |config|
  config.vm.box = "generic/openbsd7"
  config.vm.define "openbsd"
  config.vm.synced_folder ".", "/build", type: "rsync", rsync__exclude: [".git", ".direnv", "compile_commands.json"]
  config.vm.provision "shell", inline: "pkg_add -v git python-3.11.5 ninja && python3.11 -m pip install meson"
end
