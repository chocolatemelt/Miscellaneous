% setup the 5 qualities of each office
persons(0, []) :- !.
persons(N, [(_Major,_Poster,_Pizza,_Book,_Club)|T]) :- N1 is N-1, persons(N1,T).

% predicates to end the search.
% this one stops when we've reached a good element
person(1, [H|_], H) :- !.

% this one gets the Nth element of the recursive list
person(N, [_|T], R) :- person(N1, T, R), N is N1+1.

% hints.
hint1([(architecture,ctrlaltdel,_,_,_)|_]).
hint1([_|T]) :- hint1(T).

hint2([(cse,_,_,_,flying)|_]).
hint2([_|T]) :- hint2(T).

hint3([(gs,_,_,scifi,_)|_]).
hint3([_|T]) :- hint3(T).

hint4([(_,dilbert,_,_,_),(_,calvinhobbes,_,_,_)|_]).
hint4([_|T]) :- hint4(T).

hint5([(_,dilbert,_,fantasy,_)|_]).
hint5([_|T]) :- hint5(T).

hint6([(_,_,pepperoni,_,rcos)|_]).
hint6([_|T]) :- hint6(T).

hint7([(_,xkcd,cheese,_,_)|_]).
hint7([_|T]) :- hint7(T).

hint8(Students) :- person(3, Students, (_,_,_,fiction,_)).

hint9(Students) :- person(1, Students, (cs,_,_,_,_)).

hint10([(_,_,buffalochicken,_,_),(_,_,_,_,rgaming)|_]).
hint10([(_,_,_,_,rgaming),(_,_,buffalochicken,_,_)|_]).
hint10([_|T]) :- hint10(T).

hint11([(_,_,_,_,csclub),(_,_,cheese,_,_)|_]).
hint11([(_,_,cheese,_,_),(_,_,_,_,csclub)|_]).
hint11([_|T]) :- hint11(T).

hint12([(_,_,hawaiian,poetry,_)|_]).
hint12([_|T]) :- hint12(T).

hint13([(itws,_,broccoli,_,_)|_]).
hint13([_|T]) :- hint13(T).

hint14([(cs,_,_,_,_),(_,phd,_,_,_)|_]).
hint14([(_,phd,_,_,_),(cs,_,_,_,_)|_]).
hint14([_|T]) :- hint14(T).

hint15([(_,_,buffalochicken,_,_),(_,_,_,history,_)|_]).
hint15([(_,_,_,history,_),(_,_,buffalochicken,_,_)|_]).
hint15([_|T]) :- hint15(T).

% the question - who's in the taekwondo club?
question([(_,_,_,_,taekwondo)|_]).
question([_|T]) :- question(T).

% the solution, by supplying hints
solution(Persons) :-
  persons(5, Persons),
  hint1(Persons),
  hint2(Persons),
  hint3(Persons),
  hint4(Persons),
  hint5(Persons),
  hint6(Persons),
  hint7(Persons),
  hint8(Persons),
  hint9(Persons),
  hint10(Persons),
  hint11(Persons),
  hint12(Persons),
  hint13(Persons),
  hint14(Persons),
  hint15(Persons),
  question(Persons).

student(Major) :- solution(Persons), person(Number, Persons, (Major,Poster,Pizza,Book,taekwondo)).