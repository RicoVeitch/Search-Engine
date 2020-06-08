<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<html>
<head><title>Seach Engine test page</title>

</head>

<body>
<h1>Seach Engine test page</h1>

<!-- <p>Showing contents of papers table:</p> -->

<table border="1">
<form autocomplete="off" action="" method="POST">
<label for="query">Query: </label>
  <input id="query", name="query">
  <input type="submit" name="submitbutton" value="Submit"/>
</form>

<?php
   $query= $_POST['query'];
   $submitbutton= $_POST['submitbutton'];
   $port = '10002';
   $ip_address = '192.168.2.10';

   if ($submitbutton && !empty($query)){
        echo "submited";
        error_reporting(E_ALL);

        if (($client_socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
            die("socket_create() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
        }

        if(socket_connect($client_socket , $ip_address, $port) === false){
            die("socket_connect() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
        }
        
        if(socket_send($client_socket , $query , strlen($query) , 0) === false){
            die("socket_send() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
        }
   }
?>