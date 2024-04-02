Vagrant.configure("2") do |config|
  config.vm.box = "generic/ubuntu2004"
  config.vm.define "ubuntu2004"
  config.vm.synced_folder ".", "/build", type: "rsync", rsync__exclude: [".git", ".direnv", "compile_commands.json"]
  config.vm.provision "shell", inline: <<-EOF
    apt-get update
    apt-get install -y gcc g++ ninja-build python3-pip
    pip install meson
  EOF
end
