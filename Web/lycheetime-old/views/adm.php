<?php
/**
 * @author Kevin Zhang
 * admin page - manage cms and stuff
 * redirect to index if a session doesnt exist, otherwise display the site
 */

$conf = require('config.php');
if(!isset($_SESSION)) session_start();
if(!isset($_SESSION[$conf['ga_secret']])) header('Location: ./');

?>

<!DOCTYPE html>
<html>
<head>
	<title><?php echo $page_title; ?></title>
	<meta name="robots" content="noindex, nofollow" />
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
    <link rel="stylesheet" type="text/css" href="styles/reset.css" />
    <link rel="stylesheet" type="text/css" href="http://fonts.googleapis.com/css?family=Fira+Mono" />
    <link rel="stylesheet" type="text/css" href="../styles/adm.css" />
</head>
<body>
	<div id="bigbox">
		<h1>return array(</h1>
		<form id="auth" action="save.php" method="post">
			<input type="text" name="username" placeholder="<?php echo $conf['username']; ?>" maxlength="20">
			<input type="text" name="password" placeholder="db_pass" maxlength="20">
			<input type="text" name="hostname" placeholder="<?php echo $conf['hostname']; ?>" maxlength="20">
			<input type="text" name="database" placeholder="<?php echo $conf['database']; ?>" maxlength="20">

			<label>site_down =</label>
			<label>
				<input type="radio" name="site_down" value="true" <?php if($conf['site_down']) echo 'checked'; ?>>true
			</label>
			<label>
				<input type="radio" name="site_down" value="false" <?php if(!$conf['site_down']) echo 'checked'; ?>>false
			</label>
			<br>
			<label>debug =</label>
			<label>
				<input type="radio" name="debug" value="true" <?php if($conf['debug']) echo 'checked'; ?>>true
			</label>
			<label>
				<input type="radio" name="debug" value="false" <?php if(!$conf['debug']) echo 'checked'; ?>>false
			</label>
			<input type="text" name="sqrl" placeholder="<?php echo $conf['sqrl']; ?>" maxlength="20" class="last">
			<input type="submit" name="submit" value=");">
		</form>
	</div>
</body>
</html>