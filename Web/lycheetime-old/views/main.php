<?php include('header.php'); ?>
	<div id="loader-wrapper">
		<div id="loader"><p>&#30005;</p></div>
		<div class="loader-section section-left"></div>
		<div class="loader-section section-right"></div>
	</div>

	<div id="fullpage">
		<div id="splash" class="section" data-anchor="main">
			<a href="#about">
				<div id="splash-circle">
					<h1>&#33620;&#26525;</h1>
				</div>
			</a>
			<div id="scroll">
				<h2>scroll</h2>
			</div>
		</div>

		<div id="intro" class="section" data-anchor="about">
			<div class="vertical-wrap">
				<h1>hey there,</h1>
				<p>i'm kevin zhang, a developer <span class="ampersand">&amp;</span> designer based in new england.
				i make pretty things sometimes.
				<p>
				check out my <a href="resume.pdf">resume</a>, hit me up on <a href="https://github.com/chocolatemelt">github</a>, or <a href="#work">peep my latest work</a>.</p>
			</div>
		</div>

		<div id="gallery" class="section" data-anchor="work">
			<div id="gallery-container">
				<div id="gallery-header">
					<h1>gallery.</h1>
					<ul id="gallery-filters">
						<li class="tag"><a class="design">design.</a></li>
						<li class="tag"><a class="devel">development.</a></li>
						<li class="tag"><a class="misc">misc.</a></li>
					</ul>
					<hr class="begin">
				</div>
				<div id="wall">
					<div class="gutter"></div>
					<div class="brick tag-devel"><img src="gallery/placeholder.jpg" /></div>
					<div class="brick w2 tag-design"><img src="gallery/jalapeno.jpg" /></div>
					<div class="brick tag-misc"><img src="gallery/wristrest.jpg" /></div>
					<div class="brick tag-design"><img src="gallery/icai.jpg" /></div>
					<div class="brick tag-misc"><img src="gallery/rainmeter.jpg" /></div>
					<div class="brick tag-design"><img src="gallery/rpismash.jpg" /></div>
					<div class="brick tag-design"><img src="gallery/geometrika.jpg" /></div>
				</div>
			</div>
		</div>
	</div>
<?php include('footer.php'); ?>