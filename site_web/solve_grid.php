<?php

if (!isset($_POST["size"]) or !isset($_POST["penality"]) or !isset($_POST["1"])) {
    exit;
}

$size = intval($_POST["size"]);
$penality = intval($_POST["penality"]);
$gridToBeWriten = "";
// $gridStr = $grid["grid"];.

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
echo $i;

$filename = "grid" . strval($i);

$file = fopen($filename, "w");

fwrite($file, $size . ' ');
fwrite($file, $penality . "\n");
fwrite($file, $gridToBeWriten);
fclose($file);

// exec("./theGrid -o gridS" . $i . " " . $filename);

// unlink($filename);



/* 

file_put_contents($filename, $size, FILE_APPEND);
file_put_contents($filename, $penality, FILE_APPEND);
file_put_contents($filename, $gridStr, FILE_APPEND); */

/* $grid = array_fill(0, $size, array_fill(0, $size, 0));


for ($i = 0; $i < $size; $i++) {
    $grid[$i / $size][$i % $size] = $gridStr[$i];
} */