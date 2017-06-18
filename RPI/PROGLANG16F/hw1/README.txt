Alex Shin and Kevin Zhang

Features
==============================================
+ Alpha renaming
+ Beta reduction
+ Eta reduction


Bugs
==============================================
- Not necessarily a bug but the program
  assumes that the inputs are all well-formed
  lambda expressions; generally this means
  that in the case an input is not an actual
  lambda expression, the user receives no
  indication that this is incorrect
  i.e.
  	Input 1: x
  	Result 1: x

  	is the output for an input file
  	that is simply
  	x y z