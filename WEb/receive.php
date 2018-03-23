<?php

$text = $_REQUEST['value'];

$fp = fopen('text.txt', 'w');
fwrite($fp, $text);
fclose($fp);


?>