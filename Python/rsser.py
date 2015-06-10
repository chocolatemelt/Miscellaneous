#!/usr/bin/env python
# RSSer
# Downloads from RSS via urllib
# Requires feedparser python package.
# Checks for the feed items <link> and <title>.
# NOTE: Retired as of 6/10/15. This doesn't work well and the Bash script nyaa
# in this repository works better and faster.
import feedparser, urllib, sys

# handle bad cases
if(len(sys.argv) != 2):
    print("Usage: rsser URL")
    sys.exit()

# get the feed
rss_feed = feedparser.parse(sys.argv[1])

# download
for item in rss_feed.entries:
	name = item.title + ".torrent"
	urllib.urlretrieve(item.link, name)
