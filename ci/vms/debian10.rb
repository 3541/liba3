Vagrant.configure("2") do |config|
  config.vm.box = "generic/debian10"
  config.vm.define "debian10"
  config.vm.synced_folder ".", "/build", type: "rsync", rsync__exclude: [".git", ".direnv", "compile_commands.json"]
  config.vm.provision "shell", inline: <<-EOF
    apt-get update
    apt-get install -y python3-pip ninja-build
    python3 -m pip install meson
  EOF
end
