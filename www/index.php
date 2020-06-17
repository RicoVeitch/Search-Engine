<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<html>
<head><title>Seach Engine test page</title>

</head>

<body>
<h1>Seach Engine test page</h1>

</style>
<link rel="stylesheet" href="//code.jquery.com/ui/1.11.4/themes/smoothness/jquery-ui.css">
<script src="https://code.jquery.com/jquery-1.12.4.js"></script>
  <script src="https://code.jquery.com/ui/1.12.1/jquery-ui.js"></script>
  <script>
  $( function() {
    $( "#query" ).autocomplete({
      source: 'recommend.php'
    });
  } );
</script>

<form autocomplete="off" action="" method="POST">
<label for="query">Query: </label>
  <input id="query", name="query">
  <input type="submit" name="submitbutton" value="Submit"/>
</form>
<link rel="stylesheet" type="text/css" href="main.css">
<?php
  $query= $_POST['query'];
  $submitbutton= $_POST['submitbutton'];
  $port = '10002';
  $ip_address = '192.168.2.10';
  $doc_id_len = 10;

  // if (isset($_GET['reset'])) {
  //   header('Location: test.xml');
  // }

  $clicked = isset($_GET['reset']);
  if (($submitbutton && !empty($query)) || $clicked){ // || $clicked
    error_reporting(E_ALL);

    if (($client_socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
        die("socket_create() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
    }

    if(socket_connect($client_socket , $ip_address, $port) === false){
        die("socket_connect() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
    }

    if($clicked){
      $doc = $_GET['reset'];

      if(socket_send($client_socket , "-d".$doc , strlen($doc) + 2 , 0) === false){
        die("socket_send() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
      }

      if(socket_recv($client_socket , $buf , 10240, MSG_WAITALL ) === FALSE){
        die("socket_recv() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
      }
      $fp = fopen('test.xml', 'w');
      fwrite($fp, $buf);
      fclose($fp);
      header('Location: test.xml');

    }else{
      if(socket_send($client_socket , "-f".$query , strlen($query) + 2 , 0) === false){
        die("socket_send() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
      }

      if(socket_recv($client_socket , $buf , 10240, MSG_WAITALL) === FALSE){
          die("socket_recv() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
      }
      // echo nl2br("\n" . $buf);
      $results = explode("\n", $buf);
      foreach($results as $doc){
        if($doc === "No Results Found"){
          echo $doc;
          break;
        }
        $doc_id = substr($doc, 0, $doc_id_len);
        $contents = substr($doc, $doc_id_len, strlen($doc));
        echo "<a style='text-align: right;cursor:pointer;' href='index.php?reset=$doc_id' name='reset' class='system list-group-item'> $doc_id </a> <br>";
        echo $contents;
        echo "<br>";
      }

      $db_host = '192.168.2.12';
      $db_name = 'fvision';
      $db_user = 'webuser';
      $db_passwd = 'bad_db_pwd';
      $pdo_dsn = "mysql:host=$db_host;dbname=$db_name";
      $pdo = new PDO($pdo_dsn, $db_user, $db_passwd);
      // $q = $pdo->query("SELECT * FROM history");
      // while($entry = $q->fetch()){
      //     echo nl2br($entry['query'].": ".$entry['q_count']."\n");
      // }
      
      $update = "INSERT INTO history VALUES ('$query', 1) ON DUPLICATE KEY UPDATE q_count = q_count + 1";
      $q = $pdo->query($update);
    }
   }
?>