% (Major, Poster, Pizza, Genre, Club)
students(Office) :-
	member((architecture,ctrlaltdel,_,_,_), Office),
	member((cse,_,_,_,rpiflying), Office),
	member((gs,_,_,scifi,_), Office),
	next((_,dilbert,_,_,_), (_,calvinhobbes,_,_,_), Office),
	member((_,dilbert,_,fantasy,_), Office),
	member((_,_,pepperoni,_,rcos), Office),
	member((_,xkcd,cheese,_,_), Office),
	nth1(3, Office, (_,_,_,fiction,_)),
	nth1(1, Office, (cs,_,_,_,_)),
	next((_,_,buffalochicken,_,_), (_,_,_,_,rgaming), Office),
	next((_,_,_,_,csclub), (_,_,cheese,_,_), Office),
	member((_,_,hawaiian,poetry,_), Office),
	member((itws,_,broccoli,_,_), Office),
	next((cs,_,_,_,_), (_,phd,_,_,_), Office),
	next((_,_,buffalochicken,_,_), (_,_,_,history,_), Office).

% provides "next to"
next(A, B, Ls) :- append(_, [A,B|_], Ls).
next(A, B, Ls) :- append(_, [B,A|_], Ls).

student(Major) :-
	length(Office, 5),
	students(Office),
	member((Major,_,_,_,taekwondo), Office).
