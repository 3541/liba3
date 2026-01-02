Vagrant.configure("2") do |config|
  config.vm.box = "bento/freebsd-13"
  config.vm.define "freebsd"
  config.vm.synced_folder ".", "/build", type: "rsync", rsync__exclude: [".git", ".direnv", "compile_commands.json", "bazel-*", "external"]
  config.vm.provision "shell", inline: "pkg install -y git bash zip openjdk21 && git clone --depth 1 https://git.FreeBSD.org/ports.git /usr/ports"
end
