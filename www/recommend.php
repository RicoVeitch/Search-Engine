<?php
    $db_host   = '192.168.2.12';
    $db_name   = 'fvision';
    $db_user   = 'webuser';
    $db_passwd = 'bad_db_pwd';
    
    $pdo_dsn = "mysql:host=$db_host;dbname=$db_name";
    $pdo = new PDO($pdo_dsn, $db_user, $db_passwd);

    $searchTerm = $_GET['term'];
    // mysql_real_escape_string, '%s%%'
    $sql =  sprintf("SELECT * FROM history WHERE query LIKE '%s%%' ORDER BY q_count DESC", $searchTerm);
    
    $query = $pdo->query($sql);

    while ($row = $query->fetch()) {
        $data[] = $row['query'];
    }

    echo json_encode($data);
?>