-module(main).
-compile(export_all).

% seq_count assigns numbers to each of the 4 chemical bases, listed as [a, g, c, t] for
% adenosine, guanine, cytosine, and thymine respectively.
% we simply add the index we find the chemical base at to it's array, thus 'counting' it
% and storing the order we find them in so we can assign them later and perform a
% distributed merge sort
seq_count([], Counts, _) ->
	Counts;
seq_count(TargetSeq, Counts, Index) ->
	% recursively look at the first element of the given sequence and increment the array
	% count based on what we're looking at
	% [[a], [g], [c], [t]]
	case lists:nth(1, TargetSeq) of
		$a -> seq_count(string:sub_string(TargetSeq, 2), [lists:append([lists:nth(1, Counts), [Index]]), lists:nth(2, Counts), lists:nth(3, Counts), lists:nth(4, Counts)], Index + 1);
		$g -> seq_count(string:sub_string(TargetSeq, 2), [lists:nth(1, Counts), lists:append([lists:nth(2, Counts), [Index]]), lists:nth(3, Counts), lists:nth(4, Counts)], Index + 1);
		$c -> seq_count(string:sub_string(TargetSeq, 2), [lists:nth(1, Counts), lists:nth(2, Counts), lists:append([lists:nth(3, Counts), [Index]]), lists:nth(4, Counts)], Index + 1);
		$t -> seq_count(string:sub_string(TargetSeq, 2), [lists:nth(1, Counts), lists:nth(2, Counts), lists:nth(3, Counts), lists:append([lists:nth(4, Counts), [Index]])], Index + 1);
		% covers error checking for non 'agct' characters, returning a sentinel list that we
		% consider to be an errored output
		_Else ->
			io:format("Only 'a', 'g', 'c', 't' chemical bases are valid.~n"),
			[[x],[x],[x],[x]]
	end.

% seq_assign assigns numbers to each of the 4 chemical bases of an input sequence, given
% the array of chemical bases with associated indexes. basically, we look through the
% input sequence and attach the numbers we received from the above seq_count function,
% thus ending up with an unsorted list that we can then perform a distributed mergesort
% on.
seq_assign([], _) ->
	[];
seq_assign(InputSeq, Counts) ->
	% recursively look at the first element of the input sequence and append to the return
	% list the first index given by the Counts list, which is an array of arrays containing
	% the indices of the "sorted" target sequence
	case lists:nth(1, InputSeq) of
		$a -> lists:append([lists:nth(1, lists:nth(1, Counts))], seq_assign(string:sub_string(InputSeq, 2), [element(2, lists:split(1, lists:nth(1, Counts))), lists:nth(2, Counts), lists:nth(3, Counts), lists:nth(4,Counts)]));
		$g -> lists:append([lists:nth(1, lists:nth(2, Counts))], seq_assign(string:sub_string(InputSeq, 2), [lists:nth(1, Counts), element(2, lists:split(1, lists:nth(2, Counts))), lists:nth(3, Counts), lists:nth(4,Counts)]));
		$c -> lists:append([lists:nth(1, lists:nth(3, Counts))], seq_assign(string:sub_string(InputSeq, 2), [lists:nth(1, Counts), lists:nth(2, Counts), element(2, lists:split(1, lists:nth(3, Counts))), lists:nth(4,Counts)]));
		$t -> lists:append([lists:nth(1, lists:nth(4, Counts))], seq_assign(string:sub_string(InputSeq, 2), [lists:nth(1, Counts), lists:nth(2, Counts), lists:nth(3, Counts), element(2, lists:split(1, lists:nth(4, Counts)))]))
	end.

% do_work initializes the actors and begins merge sorting.
% performed in a distributed fashion by splitting the lists (a la standard merge sort)
% and spawning new actors to handle lists, down to 2 element lists.
% SeqLetter merely identifies the sequence for pattern matching down the road.
do_work(Seq, SeqLetter, Parent) ->
	case length(Seq) < 2 of
		% base case when we've reduced sequence to 2ples; we begin sorting with 0 inversions
		true -> Parent ! {Seq, SeqLetter, 0};
		% split the sequence in half (as best we can), and distribute among children
		false ->
			{SeqA, SeqB} = lists:split(trunc(length(Seq) / 2), Seq),
			spawn(main, do_work, [SeqA, a, self()]),
			spawn(main, do_work, [SeqB, b, self()]),
			% mergesort with the two initially empty (unsorted, unmerged) lists
			dist_mergesort(Parent, [], [], SeqLetter, 0, 0)
	end.

% dist_mergesort performs mergesorting in a distributed fashion. it does this by sending
% messages to the parent and receiving messages from children.
% base case when 2 messages are received from children, i.e. merging done
dist_mergesort(Parent, SeqA, SeqB, SeqLetter, Inversions, 2) ->
	MergedSeq = merge(SeqA, SeqB),
	Parent ! {element(1, MergedSeq), SeqLetter, element(2, MergedSeq) + Inversions},
	Inversions;

% distribute mergesorts among children
% since we have two sequences, we handle each one separately by
% pattern matching against the sequence identifier, A or B
dist_mergesort(Parent, SeqA, SeqB, SeqLetter, Inversions, Msgs) ->
	receive
		{RcvdSeq, a, InversionsA} ->
			dist_mergesort(Parent, RcvdSeq, SeqB, SeqLetter, Inversions + InversionsA, Msgs + 1);
		{RcvdSeq, b, InversionsB} ->
			dist_mergesort(Parent, SeqA, RcvdSeq, SeqLetter, Inversions + InversionsB, Msgs + 1)
	end.

% performs merging on two sorted sequences, calculating inversions along the way
% base cases...
merge(SeqA, []) -> {SeqA, 0};
merge([], SeqB) -> {SeqB, 0};
% and standard mergesorting, comparing heads of SeqA and SeqB
merge([Ha|Ta], [Hb|Tb]) ->
	case Ha < Hb of
		true ->
			Ret = merge(Ta, [Hb|Tb]),
			{[Ha|element(1, Ret)], element(2, Ret)};
		false ->
			Ret = merge([Ha|Ta], Tb),
			{[Hb|element(1, Ret)], element(2, Ret) + length([Ha|Ta])}
	end.


start() ->
  {ok, [StartSeq]} = io:fread("", "~s"),
  {ok, [TargetSeq]} = io:fread("", "~s"),
	% initial check to make sure we're comparing two valid inputs (same letters and count)
  case lists:sort(StartSeq) == lists:sort(TargetSeq) of
  	true ->
			Counts = seq_count(TargetSeq, [[],[],[],[]], 1), % 4 empty arrays for a, g, c, t
			% secondary check to make sure we're comparing 'agct' sequences only
			case Counts == [[x],[x],[x],[x]] of
				true ->
					io:format("The target sequence is not achievable.~n");
				% if all goes well, sort and print inversions
				false ->
					UnsortedSeq = seq_assign(StartSeq, Counts),
					spawn(main, do_work, [UnsortedSeq, a, self()]),
					receive
						{_, _, Inversions} -> io:fwrite("~B~n", [Inversions])
					end
			end;
		false ->
			io:format("The target sequence is not achievable.~n")
  end.
