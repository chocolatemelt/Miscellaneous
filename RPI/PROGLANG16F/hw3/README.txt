Alex Shin & Kevin Zhang

Features:
The facts are always assumed.
Parses hints and asserts them as rules/facts.

Bugs:
Parsing in part 2 goes wrong somewhere (either doesn't parse correctly or perhaps at all), and thus the program
returns several "possible" majors rather than the right major (the right major is included in the list of possiblities,
this furthers supports our intuition, that several of the constraints are not being applied correctly in the database).

Grammar:
Our natural language processing defines DCGs for various parts of a sentence, such as articles, nouns, verbs, prepositions,
and pronouns, then basically defines a number of possible formats of a hint and puts the relevant portions of the hint
into the list, asserting that each fact is true. If the hints contradict, then it'll return false.
Currently, the only grammar allowed is grammar as seen in the original 15 hints; if a hint were to be written in a different
format, or using a synonym not used in the 15 hints, then it'll fail as well.
