
	<script src="js/jquery.min.js"></script>
	<script src="js/jquery.easings.min.js"></script>
	<script src="js/ie10-viewport-bug-workaround.js"></script>
<?php if(isset($script)) : ?>
<?php foreach($script as $s) { ?>
	<script src="js/<?php echo $s ?>"></script>
<?php } ?>
<?php endif; ?>
</body>
</html>