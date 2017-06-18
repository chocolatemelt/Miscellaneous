// scroll effects all in one function!
function scrollfx() {
	var window_top = $(window).scrollTop();

	// fade out on scroll
	var h = $(window).height() / 3;
	var o = (150 - (window_top - h)) / 150;
	$('#scroll').css({'opacity': o});
/*
	// decrease size on scroll
	var wh = 15 * o * 1.2;
	if(wh > 15) wh = 15;
	if(wh < 5) wh = 5;
	$('#lychee').css({'width': wh + 'em', 'height': wh + 'em'});
	$('#anchor').css({'width': wh + 'em', 'height': wh + 'em'});
	$('#lychee img').css({'width': wh + 'em', 'height': wh + 'em'});

	// effects upon scrolling past splash screen
	var anchor = $('#anchor').offset().top;
	if (window_top > anchor) {
		$('#lychee').addClass('stick');
		$('nav').css({'background-color': '#fff'});
	}
	else {
		$('#lychee').removeClass('stick');
		$('nav').css({'background-color': 'transparent'});
	}*/
}

$(document).ready(function() {
	// preloader because this site is mad kilobytes
	$("body").imagesLoaded(function() {
		$('body').addClass('loaded');
	})

	/* testing! 
	setTimeout(function() {
		$('body').addClass('loaded');
	}, 3000); */

	// initialize fullpaging
	$('#fullpage').fullpage({
		scrollingSpeed: 700,
		autoScrolling: false,
		fitToSection: false,
		afterRender: function() {}
	});

	// initialize masonry
	$('#wall').masonry({
		itemSelector: '.brick',
		columnWidth: '.gutter',
		gutter: 24,
		percentPosition: true,
	})
	.imagesLoaded(function() {
		$('#wall').masonry('layout');
	});

	// scroll effect
	$(window).on('scroll', scrollfx);

	// filter toggle function
	function toggleFilters(tag, state) {
		tag_class = ".tag-" + tag;
		if(state == 'active') {
			$(tag_class).show();
			$('#wall').masonry('layout');
		} else {
			$(tag_class).hide();
			$('#wall').masonry('layout');
		}
	}

	// image selection function
	function showFilters(tag, active) {
		tag_class = ".tag-" + tag;
		if(active) {	
			$('.brick').fadeToggle(250);
			$(tag_class).fadeIn(250);
			setTimeout(function() {
				$('#wall').masonry('layout');
			}, 300);
		} else {
			$('.brick').fadeOut(250);
			$(tag_class).fadeIn(250);
			setTimeout(function() {
				$('#wall').masonry('layout');
			}, 300);
		}
	}

	// tag filtering
	$('li.tag').click(function() {
		var tag = $(this).children('a');
		var tags = $('#gallery-filters').children('li.tag').children('a');
		if(tag.hasClass('inactive')) {
			tags.each(function() {
				$(this).addClass('inactive');
			});
			tag.removeClass('inactive');
			showFilters(tag.attr('class'), false);
		} else {
			tags.each(function() {
				$(this).toggleClass('inactive');
			})
			tag.removeClass('inactive');
			showFilters(tag.attr('class'), true);
		}
		/*
		if(tag.hasClass('inactive')) {
			tag.removeClass('inactive');
			toggleFilters(tag.attr('class'), 'active');
		}
		else {
			toggleFilters(tag.attr('class'), 'inactive');
			tag.addClass('inactive');
		}
		*/
	});
});