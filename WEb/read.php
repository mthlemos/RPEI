<?php

$myfile = fopen("text.txt", "r") or die("Unable to open file!");
echo '{';
echo fread($myfile,filesize("text.txt"));
echo '}';
fclose($myfile);

?>