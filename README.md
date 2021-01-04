# BM25 search engine.
Indexed on the TREC WSJ collection. Uses three virtual machines that interact with
each other over a private network. These machines include a webpage, a database, and the 
C++ search engine.

## Getting started
Follow the steps below to run the program.

### Prerequisites
- Download and install Vagrant https://www.vagrantup.com/downloads.html
- Download and install Virtualbox https://www.virtualbox.org/wiki/Downloads

### Setting up
- After installing open terminal and navigate to the root directory.
- Due to the fact that the TREC WSJ collection is copywrited, the wsj.xml file provided just acts as test material to index on. 
- Run './setup.sh' which will index the collection and set up the virtual machines
- You can view the webpage by navigating to http://127.0.0.1:8080/
- Once you are done you can close the program by 'vagrant destroy'
- If you wish to run the program again and have already indexed the collection (by running setup.sh), use 'vagrant up' to restart the vm's.
