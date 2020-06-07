Vagrant.configure("2") do |config|
    config.vm.box = "ubuntu/xenial64"
    config.vm.define "backend" do |backend|
        backend.vm.hostname = "backend"
        backend.vm.network "private_network", ip: "192.168.2.10"
        backend.vm.synced_folder ".", "/vagrant", owner: "vagrant", group: "vagrant", mount_options: ["dmode=775,fmode=777"]
        backend.vm.provision "shell", inline: <<-SHELL
            apt-get update
            apt-get install -y g++
            sudo g++ /vagrant/test_socket.cpp -o /vagrant/test
        SHELL
    end

    config.vm.define "webserver" do |webserver|
        webserver.vm.hostname = "webserver"
        webserver.vm.network "forwarded_port", guest: 80, host: 8080, host_ip: "127.0.0.1"
        webserver.vm.network "private_network", ip: "192.168.2.11"
        webserver.vm.synced_folder ".", "/vagrant", owner: "vagrant", group: "vagrant", mount_options: ["dmode=775,fmode=777"]
        webserver.vm.provision "shell", inline: <<-SHELL
            apt-get update
            apt-get install -y apache2 php libapache2-mod-php php-mysql
            cp /vagrant/website.conf /etc/apache2/sites-available/
            # activate website conf
            a2ensite website
            # disable the default website provided by Apache
            a2dissite 000-default
            # reload the webserver configuration
            service apache2 reload
            # see at http://127.0.0.1:8080/ 
        SHELL
    end
end