v = \(x:xs) -> let { f = \x -> x*x; } in (f x);

--v0 :: Int;
--v0 = let { f [a, a] = 1; } in (f [True, False]);

v2 :: String;
v2 = let { f a = a; } in (f "asd123");

v3 :: [a];
v3 = let { f (x:xs) = xs; } in (f [1, 2, 3]);

f :: Int -> Int;
f 1 = 11;
f 2 = 22;
f _ = 123;

--ff :: (Type1 (Type2 a)) -> a;

--ff2 :: (T3 (T4));

--ff3 :: (Tree a);

data Maybe a = Just a | Nothing;

a = ((\x -> \y -> x+y) 15+2*5 3*2);

--b = if True then 1 else x+y where { x = 5; y = 4; };

c = \z -> x+y+z where { x = 5; y = 4; };

d = let { z = 3; } in x+y+z where { x = 5; y = 4; };

e x = case x of { [] -> "nil"; (x:xs) -> "list"; };

foo x = [(x+y)*c | x <- [1, 2, 3], y <- [3, 2, 1], let c = 3.14, x+y > 2];