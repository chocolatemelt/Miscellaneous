<?php

require_once '../PHP/Elo.php';

echo 'running tests<br>';

echo 'running single ranking test<br>';

$leffen = new Player();
$chillin = new Player();

echo 'competitors leffen and chillin created<br>';
echo 'leffen: ' . (string) $leffen . '<br>';
echo 'chillin: ' . (string) $chillin . '<br>';

echo 'results: leffen 5-0 chillin<br>';
echo 'calculating elo<br>';

$ranker = new Elo($leffen, $chillin);
for($i = 0; $i < 5; ++$i) $ranker->rank(1, 0);

echo 'new results:<br>';
echo 'leffen: ' . (string) $leffen . '<br>';
echo 'chillin: ' . (string) $chillin . '<br>';

echo 'finish single ranking test<br>';

echo 'running multiple ranking test<br>';

$armada = new Player();
$ppmd = new Player();
$plup = new Player();
$hbox = new Player();

echo 'competitors armada, ppmd, plup, hbox created<br>';
echo 'round 1: armada 1-0 ppmd | hbox 1-0 plup<br>';
echo 'round 2: hbox 1-0 ppmd<br>';
echo 'round 3: armada 1-0 hbox<br>';

echo 'calculating elo<br>';

Elo::rankall($armada, array(1,1), array(1400,1400));
Elo::rankall($ppmd, array(0,0), array(1400,1400));
Elo::rankall($plup, array(0), array(1400));
Elo::rankall($hbox, array(1, 1, 0), array(1400, 1400, 1400));

echo 'new results:<br>';
echo 'armada: ' . (string) $armada . '<br>';
echo 'ppmd: ' . (string) $ppmd . '<br>';
echo 'plup: ' . (string) $plup . '<br>';
echo 'hbox: ' . (string) $hbox . '<br>';

?>
