data Maybe x = Just x | Nothing;

f 0 = 1;
f n = n * (f n-1);

ff = let {x 0 = 1; x 1 = 2;} in x*x;

