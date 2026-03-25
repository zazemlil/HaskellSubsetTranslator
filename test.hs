mappairs f [] ys = [];
mappairs f (x:xs) [] = [];
mappairs f (x:xs) (y:ys) = (cons (f x y) (mappairs f xs ys));

f0 [] [] = 1;
f0 xs ys = 2;

f1 = \v1 -> \v2 -> case (v1, v2) of {
    ([], []) -> 1;
    (xs, ys) -> 2;
};

foo0 x y = let {
    f3 [] [] = 1;
    f3 xs ys = 2;
} in (f3 x y);

foo1 x y = (f3 x y) where {
    f3 [] [] = 1;
    f3 xs ys = 2;
};

b = [1 |];
c = [x | x <- [1, 2, 3]];
d = [x | x <- [3, 2, 1], x < 3];
e = [x | x <- [3, 2, 1], x < 3, x > 0];
f = [x*pi | x <- [3, 2, 1], let pi = 3.14];
g = [x*pi | x <- [3, 2, 1], let pi = 3.14, x*pi < 9];