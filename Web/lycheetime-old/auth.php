<?php
/**
 * @author Kevin Zhang
 * @copyright 2015
 * authorization using ga
 * should note that this isn't super secure without a password or anything =p
 */

$conf = require('config.php');

// setup authenticator
require_once('lib/authenticator.php');
$auth = new PHPGangsta_GoogleAuthenticator(); // dat name doe

// authenticate
$res = $auth->verifyCode($conf['ga_secret'], $_POST['ga_code'], 2); // 60s clock tolerance

if($res) {
	session_start();
	$_SESSION[$conf['ga_secret']] = 'g';
	header("Location: ./adm");
}
else {
	header("Location: ./soontm");
}

?>