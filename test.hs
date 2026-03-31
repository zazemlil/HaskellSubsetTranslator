f :: Int -> Int -> Int -> Int;
f z = \x -> \y -> x*y*z;

g = (f (\x -> [1, 2]));