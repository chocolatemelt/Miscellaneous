var Telegram = require('telegram-bot');
var apis = require('../apis');
var file = require('read-file');
var fs = require("fs");
var Chance = require('chance');
var chance = new Chance();
var tg = new Telegram(apis.MoneyBoy);

var MoneyBoy = function() {
	tg.on('message', function(msg) {
	  if(!msg.text) return;

	  if(msg.text.match(/\/sheesh/)) {
		  var f = file.readFileSync('quotes.txt');
		  lines = f.split("\n");
		  num = chance.integer({min: 0, max: lines.length - 1});
		  line = lines[num];
		  line = line.replace(' | ', '\n');
		  tg.sendMessage({
		    text: line,
		    chat_id: msg.chat.id
		  });
	  }

	  if(msg.text.match(/\/add/)) {
	  	tg.sendMessage({
				text: 'Gib dein neues Zitat vom Boy ein, Syntax: LINE 1 | LINE 2',
				reply_markup: {
        	force_reply: true,
        	selective: true
      	},
      	reply_to_message_id: msg.message_id,
				chat_id: msg.chat.id
	  	});
	  }
	  if(msg.reply_to_message) {
	  	line = msg.text;
		  fs.appendFile('quotes.txt', '\n'+line)	
		  line = line.replace('|', '\n');
		  tg.sendMessage({
		    text: msg.from.username+' hat ein neues Zitat vom Boy hinzugefügt: \n"'+line+'"',
		    chat_id: msg.chat.id
		  });	  
		}	
	});

	tg.start();
}

module.exports = MoneyBoy;