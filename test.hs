f :: Int -> Int -> Int;
f 0 = \x -> 1;
f x = \y -> x*y;