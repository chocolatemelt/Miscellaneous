% (Major, Poster, Pizza, Genre, Club)
generateRiddle(File) :-
    open(File, read, Stream),

    repeat,
        read_line(Stream, Term),
        (   at_end_of_stream(Stream)
        ->  true
        ;   process([_,_,_,_,_],Term,[]),
            fail
        ),
        % this will make sure to process the last hint
        process([_,_,_,_,_],Term,[]).

% parsing is correct = write(MAJOR) will output the same major as indicated in the hint
process(Office) -->
    article, major(MAJOR), token(major), action(occupies), article, noun, prep, article, poster(POSTER), token(comic), token(poster), punc,
    { assert(member((MAJOR,POSTER,_,_,_), Office)) }.

process(Office) -->
    article, major(MAJOR), token(major), action(belongs), prep, article, club(CLUB), token(club), punc,
    { assert(member((MAJOR,_,_,_,CLUB), Office)) }.

process(Office) -->
    article, major(MAJOR), token(major), action(reads), genre(GENRE), punc,
    { assert(member((MAJOR,_,_,GENRE,_), Office)) }.

process(Office) -->
    article, noun, prep, article, poster(POSTER1), token(comic), token(poster), verb, prep, article,
    token(left), prep, article, noun, prep, article, poster(POSTER2), token(comic), token(poster), punc,
    { assert(next((_,POSTER1,_,_,_),(_,POSTER2,_,_,_), Office)) }.

process(Office) -->
    article, noun, prep, article, poster(POSTER), token(comic), token(poster), noun, action(reads), genre(GENRE), punc,
    { assert(member((_,POSTER,_,GENRE,_), Office)) }.

process(Office) -->
    article, noun, pn, action(eats), pizza(PIZZA), token(pizza), action(belongs), prep, article, club(CLUB), token(club), punc,
    { assert(member((_,_,PIZZA,_,CLUB), Office)) }.

process(Office) -->
    article, noun, prep, article, noun, prep, article, poster(POSTER), token(comic),
    token(poster), action(eats), pizza(PIZZA), token(pizza), punc,
    { assert(member((_,POSTER,PIZZA,_,_), Office)) }.

process(Office) -->
    article, noun, action(occupies), article, noun, prep, article, token(middle), action(reads), genre(GENRE), punc,
    { assert(nth1(3, Office, (_,_,_,GENRE,_))) }.

process(Office) -->
    article, major(MAJOR), token(major), action(occupies), article, token(first), noun, punc,
    { assert(nth1(1, Office, (MAJOR,_,_,_,_))) }.

process(Office) -->
    article, noun, pn, action(eats), pizza(PIZZA), token(pizza), action(occupies), article, noun,
    token(nextto), article, noun, pn, action(belongs), prep, article, club(CLUB), token(club), punc,
    { assert(next((_,_,PIZZA,_,_), (_,_,_,_,CLUB), Office)) }.

process(Office) -->
    article, noun, pn, action(belongs), prep, article, club(CLUB), token(club), action(occupies), article,
    noun, token(nextto), article, noun, pn, action(eats), pizza(PIZZA), token(pizza), punc,
    { assert(next((_,_,_,_,CLUB), (_,_,PIZZA,_,_), Office)) }.

process(Office) -->
    article, noun, pn, action(eats), pizza(PIZZA), token(pizza), action(reads), genre(GENRE), punc,
    { assert(member((_,_,PIZZA,GENRE,_), Office)) }.

process(Office) -->
    article, major(MAJOR), token(major), action(eats), pizza(PIZZA), token(pizza), punc,
    { assert(member((MAJOR,_,PIZZA,_,_),Office))    }.

process(Office) -->
    article, major(MAJOR), token(major), action(occupies), article, noun, token(nextto), article,
    noun, prep, article, poster(POSTER), token(comic), token(poster), punc,
    { assert(next((MAJOR,_,_,_,_), (_,POSTER,_,_,_), Office)) }.

process(Office) -->
    article, noun, pn, action(eats), pizza(PIZZA), token(pizza), verb, article, noun, token(neighbor),
    pn, action(reads), genre(GENRE), punc,
    { assert(next((_,_,PIZZA,_,_), (_,_,_,GENRE,_), Office))}.

% final predicate that poses the question
student(Major) :-
    length(Office, 5),
    member((Major,_,_,_,taekwondo), Office).

% needs to be declared dynamic in order to allow assertions on runtime
:- dynamic next/3.
next(A, B, Ls) :- append(_, [A,B|_], Ls).
next(A, B, Ls) :- append(_, [B,A|_], Ls).

% define various DCGs
article --> [the] | [an] | [a].
noun --> [student] | [office] | [occupant] | [one].
verb --> ['is'] | [has].
prep --> [with] | [to] | [on] | [of] | [in].
pn --> [who].

action(eats) --> [eats].
action(occupies) --> [occupies] | [occupying].
action(belongs) --> [belongs].
action(reads) --> [reads].

token(pizza) --> [pizza].
token(comic) --> [comic].
token(poster) --> [poster] | ['poster\'s'].
token(club) --> [club].
token(major) --> [major].
token(left) --> [left].
token(middle) --> [middle].
token(first) --> [first].
token(nextto) --> [next, to].
token(neighbor) --> [neighbor].

major(itws) --> [itws].
major(architecture) --> [architecture].
major(cse) --> [cse].
major(gs) --> [gs].
major(cs) --> [cs].

poster(ctrlaltdel) --> ['ctrl+alt+del'].
poster(dilbert) --> [dilbert].
poster(calvinhobbes) --> [calvin, and, hobbes].
poster(xkcd) --> [xkcd].
poster(phd) --> [phd,comics].

genre(scifi) --> [sci, fi].
genre(fantasy) --> [fantasy].
genre(fiction) --> [fiction].
genre(poetry) --> [poetry].
genre(history) --> [history].

club(flying) --> [rpi, flying].
club(rcos) --> [rcos].
club(rgaming) --> [r, gaming, alliance].
club(csclub) --> [cs].
club(taekwondo) --> [taekwondo].

pizza(broccoli) --> [broccoli].
pizza(pepperoni) --> [pepperoni].
pizza(cheese) --> [cheese].
pizza(buffalochicken) --> [buffalo,chicken].
pizza(hawaiian) --> [hawaiian].


punc --> [.] | [?].

% provided
% ===================================================================
read_line(Stream, Term) :-
        get0(Stream,C),
    read_rest(C,Term,Stream).

/* A period or question mark ends the input. */
read_rest(46,['.'],_) :- !.
read_rest(63,['?'],_) :- !.

/* Spaces and newlines between words are ignored. */
read_rest(C,Term,Stream) :- ( C=32 ; C=10 ) , !,
                     get0(Stream,C1),
                     read_rest(C1,Term,Stream).

/* Commas between words are absorbed. */
read_rest(44,[','|Term],Stream) :- !,
                             get0(Stream,C1),
                             read_rest(C1,Term,Stream).

/* Otherwise get all of the next word. */
read_rest(C,[Word|Term],Stream) :- lower_case(C,LC),
                             read_word(LC,Chars,Next,Stream),
                             name(Word,Chars),
                             read_rest(Next,Term,Stream).

/* Space, comma, newline, period or question mark separate words. */
read_word(C,[],C,_) :- ( C=32 ; C=44 ; C=10 ;
                         C=46 ; C=63 ) , !.

/* Otherwise, get characters, convert alpha to lower case. */
read_word(C,[LC|Chars],Last,Stream) :- lower_case(C,LC),
                                get0(Stream,Next),
                                read_word(Next,Chars,Last,Stream).

/* Convert to lower case if necessary. */
lower_case(C,C) :- ( C <  65 ; C > 90 ) , !.
lower_case(C,LC) :- LC is C + 32.
