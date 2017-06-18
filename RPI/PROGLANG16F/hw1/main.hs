import PA1Helper

-- eval is a driver function that performs alpha-renaming prior to any reductions
eval :: Lexp -> Lexp
eval lexp = eval' (alpha lexp)

-- helper function which recursively attempts to eta reduce until no more can be
-- made and then proceeds to eta reduce the recursive beta reductions unti no more
-- beta reductions can be mad
eval' :: Lexp -> Lexp
eval' lexp
  | eta lexp /= lexp  = eval' $ eta lexp
  | beta lexp /= lexp = eta $ eval' $ beta lexp
  | otherwise = lexp

-- beta is a driver function for performing beta reduction that calls upon
-- eval' again to ensure the components of the lambda expression are reduced
beta :: Lexp -> Lexp
beta lexp@(Atom a) = lexp
beta (Apply (Lambda (Atom a) b) c) = beta' b a c
beta (Apply a b) = Apply (eval' a) (eval' b)
beta (Lambda a b) = Lambda (eval' a) (eval' b)

-- beta' is a helper function for dealing with Apply class lambda expressions
-- that involve a Lambda class lambda expression. This function recursively
-- calls itself until the lambda expressions have all been reduced into
-- Atom class lambda expressions
beta' :: Lexp -> String -> Lexp -> Lexp
beta' (Atom a) b c
  | a == b = c
  | otherwise = Atom a
beta' (Apply a b) d e = Apply (beta' a d e) (beta' b d e)
beta' (Lambda a b) d e = Lambda (beta' a d e) (beta' b d e)

-- eta is a driver function for performing eta reduction that also calls upon
-- eval' again to ensure the components of the lambda expression are reduced
eta :: Lexp -> Lexp
eta lexp@(Atom _) = lexp
eta lexp@(Lambda a (Apply b c))
  | a == b = lexp
  | otherwise = b
eta lexp@(Apply a b)  = Apply (eval' a) (eval' b)
eta lexp@(Lambda a b) = Lambda (eval' a) (eval' b)

-- alpha is a driver function for performing alpha renaming
alpha :: Lexp -> Lexp
alpha lexp = alpha' lexp "" 0

-- alpha' is the helper function which actually performs the renaming; this
-- function calls upon its helper function to ensure that alpha renaming occurs
-- for more complex components of the lambda expressions while recursing through
-- the entire lambda expresion
alpha' :: Lexp -> String -> Integer -> Lexp
alpha' (Atom a) c n = Atom a
alpha' (Lambda (Atom a) b) c n = (alpha'' (Lambda (Atom n') (alpha' b a $ n+1 )) a n') where n' = (a++(show $ n+1))
alpha' (Apply a b) c n = Apply (alpha' a c n) (alpha' b c n)

-- alpha'' is a helper function of alpha' which also recurses until it can
-- reach an Atom class lambda expression
alpha'' :: Lexp -> String -> String -> Lexp
alpha'' (Atom a) c n
  | a == c    = (Atom n)
  | otherwise = (Atom a)
alpha'' (Lambda (Atom a) b) c n = Lambda (Atom a) (alpha'' b c n)
alpha'' (Apply a b) c n = Apply (alpha'' a c n) (alpha'' b c n)

-- some golf that binds the input and then returns it as an argument in reverse order
-- main = getLine >>= ((flip runProgram) eval)

main = do
 putStrLn "Please enter a filename containing lambda expressions:"
 getLine >>= ((flip runProgram) eval)