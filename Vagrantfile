Vagrant.configure("2") do |config|
    config.vm.box = "ubuntu/xenial64"
    config.vm.define "backend" do |backend|
        backend.vm.hostname = "backend"
        backend.vm.network "private_network", ip: "192.168.2.10"
        backend.vm.synced_folder ".", "/vagrant", owner: "vagrant", group: "vagrant", mount_options: ["dmode=775,fmode=777"]
        backend.vm.provision "shell", inline: <<-SHELL
            apt-get update
            apt-get install -y g++
            cd /vagrant
            g++ -std=c++11 search.cpp main.cpp indexer.cpp server_socket.cpp -o search_engine
            ./search_engine -s &
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

    config.vm.define "database" do |database|
        database.vm.hostname = "database"
        database.vm.network "private_network", ip: "192.168.2.12"
        database.vm.synced_folder ".", "/vagrant", owner: "vagrant", group: "vagrant", mount_options: ["dmode=775,fmode=777"]
        
        database.vm.provision "shell", inline: <<-SHELL
          apt-get update
          
          # We create a shell variable MYSQL_PWD that contains the MySQL root password
          export MYSQL_PWD='bad_root_pwd'
    
          echo "mysql-server mysql-server/root_password password $MYSQL_PWD" | debconf-set-selections 
          echo "mysql-server mysql-server/root_password_again password $MYSQL_PWD" | debconf-set-selections
    
          # Install the MySQL database server.
          apt-get -y install mysql-server
    
          # Create the database.
          echo "CREATE DATABASE fvision;" | mysql
    
          # Create a database user webuser with the password.
          echo "CREATE USER 'webuser'@'%' IDENTIFIED BY 'bad_db_pwd';" | mysql
    
          # Give all permissions to the webserver
          echo "GRANT ALL PRIVILEGES ON fvision.* TO 'webuser'@'%'" | mysql
          
          export MYSQL_PWD='bad_db_pwd'
    
          # run whats in the setup file
          cat /vagrant/query-database.sql | mysql -u webuser fvision
    
          # all database to accept connections from any network interface.
          sed -i'' -e '/bind-address/s/127.0.0.1/0.0.0.0/' /etc/mysql/mysql.conf.d/mysqld.cnf
    
          # make the changes known.
          service mysql restart
        SHELL
    end
end