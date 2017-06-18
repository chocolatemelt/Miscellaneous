import PA1Helper

-- eval is a driver function to perform alpha-renaming prior to any reductions
eval :: Lexp -> Lexp
eval x = (reduce (alpha x))

-- reduce is a function which performs reductions on the lambda expression
reduce :: Lexp -> Lexp
reduce x@(Atom _) = x
reduce (Lambda x (Apply y z))
 | x == reduce z = reduce y
 | otherwise = Lambda x $ reduce $ Apply y z
reduce (Lambda x y) = Lambda x $ reduce y
reduce (Apply x y) = reduce' (reduce x) (reduce y)

-- reduce' is a helper function which is used to handle Apply lexp lexp cases
reduce' :: Lexp -> Lexp -> Lexp
reduce' (Lambda x y) z
 | x == (reduce y) = z
 | otherwise = Apply (Lambda x y) $ reduce z
reduce' x y = Apply (reduce x) (reduce y)

-- alpha is a driver function for performing alpha renaming on the lambda expression
alpha :: Lexp -> Lexp
alpha lexp = alpha' lexp "" 0

alpha' :: Lexp -> String -> Integer -> Lexp
alpha' (Atom a) c i = Atom a
alpha' (Lambda (Atom a) b) c i = alpha'' (Lambda (Atom i') (alpha' b a (i + 1))) a i'
  where i' = a++(show (i + 1))
alpha' (Apply a b) c i = Apply (alpha' a c i) $ alpha' b c i

-- recursive helper function, rename at the atom level
alpha'' :: Lexp -> String -> String -> Lexp
alpha'' (Atom a) c i
  | a == c    = Atom i  -- if bounded variable, rename
  | otherwise = Atom a  -- else keep atom
alpha'' (Lambda a b) c i = Lambda a $ alpha'' b c i
alpha'' (Apply a b) c i = Apply (alpha'' a c i) $ alpha'' b c i

main = do
 putStrLn "Please enter a filename containing lambda expressions:"
 getLine >>= ((flip runProgram) eval)
