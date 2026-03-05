f :: Int -> Int;
f x = x*y;
f 0 = -1;
g x = let {
    pi f = (f 3.14);
    pi 0 = 0;
} in x*pi;
g 0 = -1;

pi = 3.14;
pi = 1;

data Maybe x = Just x | Nothing;