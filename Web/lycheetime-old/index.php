<?php
/**
 * @author Kevin Zhang
 * main routing engine for lycheeti.me.
 */

// set up some variables
$f3 = require('lib/base.php');
$conf = require('config.php');

// set debug level
if($conf['debug'])
	$f3->set('DEBUG', 3);
else
	$f3->set('DEBUG', 0);

// index page
$f3->route('GET @index: /', function($f3) {
	$f3->set('page_title', 'lycheetime!');
	$f3->set('style', array('main.css'));
	$f3->set('script', array('jquery.fullPage.min.js', 'masonry.pkgd.min.js', 'imagesloaded.pkgd.min.js', 'pages/main.js'));
	echo View::instance()->render('views/main.php');
});

// lab page
$f3->route('GET @lab: /lab', function($f3) {
	$f3->set('page_title', 'lycheelab!');
	$f3->set('style', array('lab.css'));
	echo View::instance()->render('views/lab.php');
});

// custom homepage
$f3->route('GET @homepage: /home', function($f3) {
	$f3->set('page_title', 'lycheetime!');
	echo View::instance()->render('views/homepage.php');
});

//login
$f3->route('GET @login: /login', function($f3) {
	$f3->set('page_title', 'lycheelog.in');
	echo View::instance()->render('views/login.php');
});

// cms administration
$f3->route('GET @adm: /adm', function($f3) {
	$f3->set('page_title', 'lychee admin');
	echo View::instance()->render('views/adm.php');
});

// custom error handling
$f3->set('ONERROR', function($f3) {
	$f3->set('error_text', $f3->get('ERROR.text'));
	$f3->set('error_code', $f3->get('ERROR.code'));
	$f3->set('page_title', 'uh oh, something went wrong');
	echo View::instance()->render('views/error.php');
});

// redirect to maintenance page when site is down
if($conf['site_down']) $f3->redirect('GET|HEAD @index', '/soontm');

$f3->run();

?>