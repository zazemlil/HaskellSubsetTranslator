data Maybe x = Just x | Nothing;

f 0 = 1;
f n = n * (f n-1);

