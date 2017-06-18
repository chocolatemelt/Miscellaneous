<?php
/**
 * @author Kevin Zhang
 * edits config file from a web interface
 */

$conf = require('config.php');

/* blank slate
if($_POST[''] != '') {
	$conf[''] = $_POST[''];
}
*/

// mysql connection variables
if($_POST['username'] != '')
	$conf['username'] = $_POST['username'];
if($_POST['password'] != '')
	$conf['password'] = $_POST['password'];
if($_POST['hostname'] != '')
	$conf['hostname'] = $_POST['hostname'];
if($_POST['database'] != '')
	$conf['database'] = $_POST['database'];

// cms settings
$conf['site_down'] = ($_POST['site_down'] == 'true' ? true : false);
$conf['debug'] = ($_POST['debug'] == 'true' ? true : false);
if($_POST['sqrl'] = '')
	$conf['sqrl'] = $_POST['sqrl']; 

// save
file_put_contents('config.php', '<?php
/**
 * @author Kevin Zhang
 * configuration file
 */

return ' . var_export($conf, true) . ';

?>');

// probably should use a callback
header('Location: ./adm');

?>