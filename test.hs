data Maybe x = Just x | Nothing;

f :: Int -> Int;
f 0 = 1;
f n = n * (f n-1);

ff :: Int;
ff = let {x 0 = 1; x 1 = 2;} in x*x;

foo (True 1 (Car)) = 1;
foo (True 2 (Car)) = 2;

e = y*x where {f 0 = 5; f 1 = 2;};