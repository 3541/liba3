Vagrant.configure("2") do |config|
  config.vm.box = "generic/openbsd7"
  config.vm.synced_folder ".", "/src", type: "rsync", rsync__exclude: [".git", ".direnv", ".cache", "compile_commands.json"]
  config.vm.provision "shell", inline: <<-END
    pkg_add meson ninja
  END
end
