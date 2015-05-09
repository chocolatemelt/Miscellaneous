<?php
/*
 * This tiny script just iterates through the folder it's in and returns a random PNG image.
 */

$images = array();

/* glob is the fun and easy way to do it - but very slow! - http://www.phparch.com/2010/04/putting-glob-to-the-test/
 * leaving this here because it's interesting
foreach (glob("*.png") as $filename) {
	$images[] = $filename;
}*/

/* I don't want to sacrifice performance for smaller code, so the actual implementation will use opendir() */
$dir = opendir('.');
while(($filename = readdir($dir)) !== false) {
	$info = pathinfo($filename);
	if ($filename == '.' or $filename == '..' or $info['extension'] != 'png') continue;
	$images[] = $filename;
}
closedir($dir);

/* now get the image and redirect to it */
$random = $images[array_rand($images)];
header('Location: ' . $random);

?>