# salty-elo
Libraries written for Elo-based ranking systems, written with extensibility and ease-of-use in mind. I apologize in advance for my low-elo naming skills.

Based on FIDE standards, found in the rulebook [here](http://www.fide.com/component/handbook/?id=172&view=article). Calculations from [Wikipedia](https://en.wikipedia.org/wiki/Elo_rating_system).

# contributing
I'm awful at programming! If you want to fix anything up please send a pull request.

If you want to port the library to another language just add it to its own folder as the language name (i.e. C++/ or Javascript/). Similarly, adding a test file is as simple as putting it into the tests/ folder as test.ext (i.e. test.cpp or test.js).

# todo / thoughts
  - make Elo class more generic?
  - optimize functions
  - C/++ implementation

# examples
PHP

```
$leffen = new Player();
$chillin = new Player();
$ranking = new Elo($leffen, $chillin);
$ranking->rank(1, 0); // my b
```
