<!DOCTYPE html>
<html>
<head>
	<title><?php echo $page_title; ?></title>
	<meta name="robots" content="noindex, nofollow" />
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
    <link rel="stylesheet" type="text/css" href="styles/reset.css" />
    <link rel="stylesheet" type="text/css" href="http://fonts.googleapis.com/css?family=Fira+Mono" />
    <link rel="stylesheet" type="text/css" href="../styles/login.css" />
</head>
<body>
	<div id="bigbox">
		<h1>$ga_code =</h1>
		<form id="auth" action="auth.php" method="post">
			<input type="text" name="ga_code" maxlength="6">
			<input type="submit" name="submit" value=";">
		</form>
	</div>
</body>
</html>