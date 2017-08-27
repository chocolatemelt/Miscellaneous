// find font in use (fallback is the included helvetica)
d = new Detector();
font = '';
if(d.detect('Helvetica Neue') || d.detect('HelveticaNeue') || d.detect('Helvetica Neue LT Std')) {
	font = 'Helvetica Neue';
} else if(d.detect('Helvetica') || d.detect('Helvetica LT Std')) {
	font = 'Helvetica';
} else {
	font = 'TeX Gyre Heros';
}

$(document).ready(function() {
	$('.watching .highlight').append(font);
});