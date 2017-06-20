var Telegram = require('telegram-bot');
var filesize = require('humanize').filesize;
var nyaa = require('nyaatorrents');
var xml2js = require('xml2js');
var request = require('request');
var _ = require('lodash');
var apis = require('../apis');
var utils = require('../utils');
var tg = new Telegram(apis.nyaa);

/* 
 * Much love to Sebmaster of #TwoDee for the logic behind this bot.
 * github.com/Sebmaster
 */

var nyaa = function() {

  // define base URLs
  var nyaaBaseURL = 'http://nyaa.se/';
  var sukebeiBaseURL = 'http://sukebei.nyaa.se/';

   // define functions
  function parse(url, cb) {
    request.get({ url: url }, function (err, res, body) {
      if(err) {
        cb(err);
        return;
      }

      var parser = new xml2js.Parser();
      parser.parseString(body.toString('utf8'), cb);
    });
  }

  function editEntryFormat(entry) {
    var re = /(\d+) seeder\(s\), (\d+) leecher\(s\), (\d+) download\(s\) - (\d+\.?\d* \S+)/
    var match = re.exec(entry.description);
    if(!match) return null;

    entry.seeders = match[1];
    entry.leechers = match[2];
    entry.downloads = match[3];
    entry.size = match[4];

    entry.age = Date.now() - new Date(entry.pubDate).getTime();

    return entry;
  }

  function formatData(data, searchURL) {
    return utils.format('{0} - {1} | S: {2} | L: {3} | {4} | {5}', data.title, data.size, data.seeders, data.leechers, data.guid, data.link);
  }

  function getBest(entries) {
    var seedersWeight = 1;
    var leechersWeight = .5;
    var ageWeight = -1 / 1000 / 60 / 60;
    var bestScore = entries[0].seeders * seedersWeight + entries[0].leechers * leechersWeight + entries[0].age * ageWeight;
    var bestEntry = entries[0];

    for(var i = 1; i < entries.length; i++) {
      var score = entries[i].seeders * seedersWeight + entries[i].age * ageWeight;
      if(score > bestScore) {
        bestScore = score;
        bestEntry = entries[i];
      }
    }

    return bestEntry;
  }

  function help(type) {
    switch(type) {
      case 'nyaa': return 'Returns a torrent link for the given name from nyaa.se for english-translated anime.\nUsage: /nyaa ANIME';
      case 'nyaall': return 'Returns a torrent link for the given name from nyaa.se.\nUsage: /nyaall NAME';
      case 'nyaan': return 'Returns a torrent link for the given name from sukebei.nyaa.se.\nUsage: /nyaan NAME';
      default: return '';
    }
  }

  // command map
  var map = {
    nyaa: function(msg) {
      // return a help if nothing specified
      if(msg.text === '/nyaa') return msg.reply({ text: help('nyaa') });

      // grab the param
      var text = msg.text.split(' ');
      text.shift();
      var param = text.join(' ');

      // parse the url
      parse(nyaaBaseURL + '?page=rss&cats=1_37&term=' + encodeURIComponent(param), function(err, data) {
        if(err || !data.rss) {
          console.error('Error getting data: ' + err);
          return msg.reply({
            text: 'No response, please try again.'
          });
        }

        var entries = data.rss.channel[0].item;
        if(!entries || entries.length === 0) {
          return msg.reply({
            text: 'No results for ' + param + '. Try /nyaall or nyaan?'
          });
        }

        entries.map(editEntryFormat);

        for (var i = 0; i < entries.length; i++) {
            if (entries[i] === null) {
              entries.splice(i, 1);
              i--;
            }
          }
        
        if(entries.length === 0) {
          return msg.reply({
            text: 'Something went wrong, please try again.'
          });
        }

        var dataString = formatData(getBest(entries));

        return msg.reply({
          text: dataString + '\n' + 'http://nyaa.se/?page=search&cats=1_37&term=' + encodeURIComponent(param)
        });
      });
    },
    nyaall: function(msg) {
      // return a help if nothing specified
      if(msg.text === '/nyaall') return msg.reply({ text: help('nyaall') });

      // grab the param
      var text = msg.text.split(' ');
      text.shift();
      var param = text.join(' ');

      // parse the url
      parse(nyaaBaseURL + '?page=rss&term=' + encodeURIComponent(param), function(err, data) {
        if(err || !data.rss) {
          console.error('Error getting data: ' + err);
          return msg.reply({
            text: 'No response, please try again.'
          });
        }

        var entries = data.rss.channel[0].item;
        if(!entries || entries.length === 0) {
          return msg.reply({
            text: 'No results for ' + param + '. Try /nyaan?'
          });
        }

        entries.map(editEntryFormat);

        for (var i = 0; i < entries.length; i++) {
          if (entries[i] === null) {
            entries.splice(i, 1);
            i--;
          }
        }

        if(entries.length === 0) {
          return msg.reply({
            text: 'Something went wrong, please try again.'
          });
        }

        var dataString = formatData(getBest(entries));

        return msg.reply({
          text: dataString + '\n' + 'http://nyaa.se/?page=search&term=' + encodeURIComponent(param)
        });
      });
    },
    nyaan: function(msg) {
      // return a help if nothing specified
      if(msg.text === '/nyaan') return msg.reply({ text: help('nyaan') });

      // grab the param
      var text = msg.text.split(' ');
      text.shift();
      var param = text.join(' ');

      // parse the url
      parse(sukebeiBaseURL + '?page=rss&term=' + encodeURIComponent(param), function(err, data) {
        if(err || !data.rss) {
          console.error('Error getting data: ' + err);
          return msg.reply({
            text: 'No response, please try again.'
          });
        }

        var entries = data.rss.channel[0].item;
        if(!entries || entries.length === 0) {
          return msg.reply({
            text: 'No results for ' + param + '. Try /nyaa or /nyaall?'
          });
        }

        entries.map(editEntryFormat);

        for (var i = 0; i < entries.length; i++) {
          if (entries[i] === null) {
            entries.splice(i, 1);
            i--;
          }
        }

        if(entries.length === 0) {
          return msg.reply({
            text: 'Something went wrong, please try again.'
          });
        }

        var dataString = formatData(getBest(entries));

        return msg.reply({
          text: '[NSFW] ' + dataString + '\n' + 'http://sukebei.nyaa.se/?page=search&term=' + encodeURIComponent(param)
        });
      });
    }
  };

  // command handler
  tg.on('message', function(msg) {
    var cmd, ref, text;
    text = String(msg.text).trim();
    msg.reply = function(options) {
      return tg.sendMessage(_.defaults(options, {
        reply_to_message_id: this.message_id,
        chat_id: this.chat.id
      }));
    };
    //console.log(msg.date + " " + msg.from.username + ": " + msg.text);
    cmd = String((ref = text.match(/^\/([a-zA-Z0-9]*)?/i)) != null ? ref[1] : void 0).toLowerCase();
    if(cmd && map[cmd]) {
      return map[cmd](msg);
    }
  });

  tg.start();

};

module.exports = nyaa;
