<!-- <!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN"> -->
<!DOCTYPE html>
<html>
<head><title>Seach Engine test page</title>
<link rel="stylesheet" type="text/css" href="mainstyle.css">
</head>

<body>
<h1>Seach Engine test page</h1>
<!-- <style>
body {
    background-color: blue;
  }
</style> -->
<link rel="stylesheet" href="//code.jquery.com/ui/1.11.4/themes/smoothness/jquery-ui.css">
<script src="https://use.fontawesome.com/releases/v5.0.13/js/all.js"></script>
<script src="https://code.jquery.com/jquery-1.12.4.js"></script>
  <script src="https://code.jquery.com/ui/1.12.1/jquery-ui.js"></script>
  <script>
  $( function() {
    $( "#query" ).autocomplete({
      source: 'recommend.php'
    });
  } );
</script>


<div class="search_box">
<form autocomplete="off" method="post" id="sendform">
  <input id="query" class="search_text" name="query" type="text">
  <!-- <input type="submit" name="submitbutton" value="Submit" class="search_button"/> -->
  <a class="search_button" href="javascript:{}" onclick="document.getElementById('sendform').submit();" id="submitbutton" name="submitbutton">
  <i class="fas fa-search"></i>
  </a>
  </form>
</div>

</body>
</html>
<?php
  $query= $_POST['query'];
  $submitbutton = !empty($_POST);
  $port = '10002';
  $ip_address = '192.168.2.10';
  $doc_id_len = 10;

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
      $css = "<?xml-stylesheet type='text/css' href='doc.css'?>";
      $fp = fopen('doc.xml', 'w');
      fwrite($fp, $css);
      fwrite($fp, $buf);
      fclose($fp);
      header('Location: doc.xml');

    }else{
      if(socket_send($client_socket , "-f".$query , strlen($query) + 2 , 0) === false){
        die("socket_send() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
      }

      if(socket_recv($client_socket , $buf , 10240, MSG_WAITALL) === FALSE){
          die("socket_recv() failed: reason: \n" . socket_strerror(socket_last_error()) . "\n");
      }
      // echo nl2br("\n" . $buf);
      $results = explode("\n", $buf);
      echo "<div class='results'>";
      foreach($results as $doc){
        if($doc === "No Results Found"){
          echo $doc;
          break;
        }
        if(strlen($doc) == 0){
          break;
        }
        $doc_id = substr($doc, 0, $doc_id_len);
        $contents = substr($doc, $doc_id_len, strlen($doc));
        echo "<a style='text-align: right;cursor:pointer;' href='index.php?reset=$doc_id' name='reset' class='system list-group-item'> WSJ$doc_id </a> ";
        echo "<p>$contents</p>";
        // echo "<br>";
      }
      echo "</div>";

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