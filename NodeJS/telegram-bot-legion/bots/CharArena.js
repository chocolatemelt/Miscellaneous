var Telegram = require('telegram-bot');
var apis = require('../apis');
var events = require('../events'); //link to events - not in this repo
var file = require('read-file');
var _ = require('lodash');
var Chance = require('chance');
var chance = new Chance();
var tg = new Telegram(apis.CharArena);
var map;

var dev = true;

var sessions = {};

var CharArena = function() {
	map = {
		help: function(msg) {
			return msg.reply({
				text: 'help me aaaaaa'
			});
		},
		stop: function(msg) {
			if(sessions[msg.chat.id]) {
				delete sessions[msg.chat.id];
				return msg.reply({
					text: 'Battle stopped'
				});
			} else {
				return msg.reply({
					text: 'There\'s no battle running in this chat'
				});
			}
		},
		start: function(msg) {
			if(sessions[msg.chat.id]) {
				return msg.reply({
					text: 'Another battle is already in action'
				});
			} else {
				var started, dead, first, second;
				var player1 = {};
				var player2 = {};

				sessions[msg.chat.id] = function(msg) {
					if(started) {
						if(!dead) {
							var rnd = chance.integer({min : 0, max: 100});
							if(rnd >= 50) {
								first = player1;
								second = player2;
							} else {
								first = player2;
								second = player1;							
							}
							var rnd2 = chance.integer({min: 0, max: events.length-1});
							var currEvent = events[rnd2].text;
							currEvent = currEvent.replace(/\[x\]/g, first.name);
							currEvent = currEvent.replace(/\[y\]/g, second.name);
							if(events[rnd2].dead) dead = true;
							tg.sendMessage({
								text: currEvent+'\n[Send a message to continue]',
								chat_id: msg.chat.id,
								reply_to_message_id: first.pic			
							});								
						} else {
							tg.sendMessage({
								text: second.name +' wins, gg! The battle has ended',
								chat_id: msg.chat.id,
								reply_to_message_id: second.pic			
							});	
							delete sessions[msg.chat.id]							
						}
					}
					if(msg.reply_to_message) {
						console.log(msg.reply_to_message.from.id+', '+player1.id);
						if(msg.from.id == player1.id && !player1.name) {
							player1.name = msg.text;
						} else if(msg.from.id == player2.id) {
							player2.name = msg.text;
						} else {
								return msg.reply({
									text: 'That\'s not your character'
								});								
						}

						if(player1.name && player2.name) {
							started = true;
							return msg.reply({
								text: 'Preparations complete, the battle can begin!\n[Send a message to start]'
							});							
						}
						return msg.reply({
							text: 'First contestant is registered, someone upload another image to start the battle'
						});					
					}
					if(msg.photo && !started) {
						if(player1.id) {
							player2.pic = msg.message_id;
							player2.id = msg.from.id;
						} else {
							player1.pic = msg.message_id;
							player1.id = msg.from.id;
						}

						return msg.reply({
							text: 'Now give your contestant a name @'+msg.from.username,
							reply_markup: {
              	force_reply: true,
              	selective: true
            	}
						});
					}
				}
				var startMessage = '';
				if(dev) { 
					startMessage += 'DEV MODE ACTIVE: The bot is currently under active development and might produce unexpected behaviour.\n\n'; 
				}
				startMessage += 'Battle preparation started, please upload an image that shall represent your contestant';
				return msg.reply({
					text: startMessage
				});
			}
		}
	};

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
		if(sessions[msg.chat.id]) {
			return sessions[msg.chat.id](msg);
		}
	});

	tg.start();
};

module.exports = CharArena;