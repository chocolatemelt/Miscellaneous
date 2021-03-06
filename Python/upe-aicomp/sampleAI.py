import requests # if not installed already, run python -m pip install requests OR pip install requests, whatever you normally do
import random #used for sample ai to select moves at random

#constants for networking
username = "kzhaaang"
devkey = "5823b8c404d7ce4a44c37ab4"
qualifierURL = "http://aicomp.io/api/games/practice" #practice qualifier bot
rankedURL = "http://aicomp.io/api/games/search" #ranked matchmaking vs other AI

def main():
    gameMode = input("Enter 1 for qualifier bot, 2 for ranked MM, anything else to abort: ").strip()
    if (not (gameMode == "1" or gameMode == "2")):
        raise Exception("Error: Invalid Game Mode. Aborting.")

    r = requests.post(qualifierURL if gameMode == "1" else rankedURL, data={'devkey': devkey, 'username': username}) # search for new game
    json = r.json() # when request comes back, that means you've found a match! (validation if server goes down?)
    print(json)

    gameID = json['gameID']
    playerID = json['playerID']
    print(gameID)
    print(playerID)
    possibleMoves = ['mu', 'ml', 'mr', 'md', 'tu', 'tl', 'tr', 'td', 'b', '', 'op', 'bp', 'buy_count', 'buy_range', 'buy_pierce', 'buy_block']
    output = {'state': 'in progress'}
    while output['state'] != 'complete':
        moveChoice = random.randint(0,len(possibleMoves)-1) #sample ai just chooses a move at random
        r = requests.post('http://aicomp.io/api/games/submit/' + gameID, data={'playerID': playerID, 'move': possibleMoves[moveChoice], 'devkey': devkey}); # submit sample move
        json = r.json()
        print(json)
        output = json

if __name__ == "__main__": #follow python standard practice in case we decide to run the module from somewhere else
    main()
