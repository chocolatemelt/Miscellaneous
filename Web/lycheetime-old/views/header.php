<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0">

	<meta name="author" content="kevin zhang">
	<meta name="description" content="hello">
	<link rel="canonical" href="http://lycheeti.me">

	<title><?php echo $page_title ?></title>

	<link rel="icon" type="image/png" href="img/favicon-16.png" sizes="16x16">
	<link rel="icon" type="image/png" href="img/favicon-32.png" sizes="32x32">
	<link rel="icon" type="image/png" href="img/favicon-96.png" sizes="96x96">

	<link rel="stylesheet" type="text/css" href="css/bootstrap.min.css">
	<link rel="stylesheet" type="text/css" href="css/jquery.fullPage.css">
	<link rel="stylesheet" type="text/css" href="css/core.css">
<?php if(isset($style)) : ?>
<?php foreach($style as $c) { ?>
	<link rel="stylesheet" type="text/css" href="css/<?php echo $c ?>">
<?php } ?>
<?php endif; ?>
	<link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Libre+Baskerville:400,400i">

	<!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->
	<!--[if lt IE 9]>
		<script src="js/html5shiv.min.js"></script>
		<script src="js/respond.min.js"></script>
	<![endif]-->
</head>
<body>