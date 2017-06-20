var Telegram = require('telegram-bot');
var xml2js = require('xml2js');
var request = require('request');
var _ = require('lodash');
var apis = require('../apis');
var utils = require('../utils');
var tg = new Telegram(apis.booru);

/* 
 * Much love to Sebmaster of #TwoDee for the logic behind this bot.
 * github.com/Sebmaster
 */

var booru = function() {

  var random = utils.random;

  function getBooru(msg, host, tags) {
    return requestAndParse(host + '/index.php?page=dapi&s=post&q=index&tags=' + encodeURIComponent(tags) + '&limit=0', function (err, res) {
      if (err || !res.posts) {
        console.error('Booru error: ' + err + ', response');
        console.log(res);
        return msg.reply({
          text: 'Error while parsing booru response.'
        })
      }

      if (res.posts.$.count === '0') {
        return msg.reply({
          text: 'Sorry, nothing found for ' + tags
        })
      }

      function retry(times) {
        var rand = random(res.posts.$.count);
        requestAndParse(host + '/index.php?page=dapi&s=post&q=index&tags=' + encodeURIComponent(tags) + '&limit=1&pid=' + rand, function (err, res) {
          if (err) {
            return msg.reply({
              text: 'Error while trying to reach booru.'
            })
          }

          if (!res.posts || !res.posts.post || !res.posts.post.length) {
            console.log(res);
            return msg.reply({
              text: 'Invalid booru reply.'
            })
          }

          request.head({ url: res.posts.post[0].$.file_url, headers: { Referer: res.posts.post[0].$.file_url } }, function (err, resp) {
            if (!err && resp.statusCode >= 200 && resp.statusCode < 300) {
              return msg.reply({
                text: (res.posts.post[0].$.rating && res.posts.post[0].$.rating !== 's' ? '[NSFW] - ' : '') + res.posts.post[0].$.file_url
              })
            } else if (times) {
              retry(times - 1);
            } else {
              return msg.reply({
                text: 'No valid link after 3 tries :('
              })
            }
          });
        });
      }

      retry(2);
    });
  }

  function requestAndParse(url, cb) {
    request.get(url, function (err, res, body) {
      if (err) {
        cb(err);
        return;
      }

      var parser = new xml2js.Parser();
      try {
        parser.parseString(body.toString('utf8'), cb);
      } catch (e) {
        cb(e);
      }
    });
  }

  function help(type) {
    switch(type) {
      case 'sb': return 'Returns a random image from safebooru.org. Extra tags are optional. Usage: /sb [TAGS...]';
      case 'gb': return 'Returns a random image from gelbooru.com. Extra tags are optional. Usage: /gb [TAGS...]';
      case 'catgirl': return 'Returns a random catgirl (tagged \'cat_ears\') from safebooru.org. Extra tags are optional. Usage: /catgirl [TAGS...]';
      default: return '';
    }
  }

  // command map
  var map = {
    sb: function(msg) {
      // return a help if nothing specified
      if(msg.text === '/sb') return msg.reply({ text: help('sb') });

      // grab the param (there's probably a better way to do this rather than copy paste)
      var text = msg.text.split(' ');
      text.shift();
      var param = text.join(' ');

      // query sb
      return getBooru(msg, 'http://safebooru.org', 'rating:safe ' + param);
    },
    gb: function(msg) {
      // return a help if nothing specified
      if(msg.text === '/gb') return msg.reply({ text: help('gb') });

      // grab the param
      var text = msg.text.split(' ');
      text.shift();
      var param = text.join(' ');

      // query gb
      return getBooru(msg, 'http://gelbooru.com', param);
    },
    catgirl: function(msg) {
      // return a help if nothing specified
      if(msg.text === '/catgirl') return msg.reply({ text: help('catgirl') });

      // grab the param
      var text = msg.text.split(' ');
      text.shift();
      var param = text.join(' ');

      // query sb
      return getBooru(msg, 'http://safebooru.org', 'rating:safe cat_ears ' + param); // i kinda want to remove the safe tag
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
    cmd = String((ref = text.match(/^\/([a-zA-Z0-9]*)?/i)) != null ? ref[1] : void 0).toLowerCase();
    if(cmd && map[cmd]) {
      return map[cmd](msg);
    }
  });

  tg.start();

};

module.exports = booru;
