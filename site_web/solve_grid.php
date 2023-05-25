<?php

if (!isset($_POST["size"]) or !isset($_POST["penality"]) or !isset($_POST["1"])) {
    exit;
}

$size = intval($_POST["size"]);
$penality = intval($_POST["penality"]);
$time = 60;
if (isset($_POST["time"]) && ($_POST["time"]) != "") {
    $time = intval(($_POST["time"]));
}
$gridToBeWriten = "";

for ($i = 1; $i <= $size * $size; $i++) {
    if (!isset($_POST[strval($i)]) or $_POST[strval($i)] == "") {
        return;
    }
    $gridToBeWriten = $gridToBeWriten . strval($_POST[strval($i)]) . " ";
    if ($i % $size == 0 and $i != 0) {
        $gridToBeWriten = $gridToBeWriten . "\n";
    }
}

$i = 1;
while (true) {
    if (!file_exists("grid" . strval($i))) {
        break;
    }
    $i += 1;
}
$dirname = "var/";
$filename = $dirname . "grid" . strval($i);

$file = fopen($filename, "w");

fwrite($file, $size . ' ');
fwrite($file, $penality . "\n");
fwrite($file, $gridToBeWriten);
fclose($file);
$output_file = $dirname . "gridS" . $i;

exec("./theGrid -t " . $time . " -o " . $output_file . " " . $filename);

unlink($filename);

echo file_get_contents($output_file);
unlink($output_file);
