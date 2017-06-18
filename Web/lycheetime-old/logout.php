<?php
/**
 * @author Kevin Zhang
 * logout and redirect to index
 */

if(!isset($_SESSION)) session_start();
unset($_SESSION[$conf['ga_secret']]);
session_destroy();
header('Location: ./');

?>