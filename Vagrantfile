Vagrant.configure("2") do |config|
    config.vm.define "webserver" do |webserver|
        webserver.vm.hostname = "webserver"
        webserver.vm.network "forwarded_port", guest: 80, host: 8080, host_ip: "127.0.0.1"
        webserver.vm.network "private_network", ip: "192.168.2.11"
        webserver.vm.synced_folder ".", "/vagrant", owner: "vagrant", group: "vagrant", mount_options: ["dmode=775,fmode=777"]
        webserver.vm.provision "shell", inline: <<-SHELL
            apt-get update
            apt-get install -y apache2 php libapache2-mod-php php-mysql
            # activate website conf
            a2ensite test-website
            # disable the default website provided by Apache
            a2dissite 000-default
            # reload the webserver configuration
            service apache2 reload
        SHELL
    end
end