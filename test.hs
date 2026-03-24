mappairs f [] ys = [];
mappairs f (x:xs) [] = [];
mappairs f (x:xs) (y:ys) = (cons (f x y) (mappairs f xs ys));

f [] [] = 1;
f xs ys = 2;

f1 = \v1 -> \v2 -> case (v1, v2) of {
    ([], []) -> 1;
    (xs, ys) -> 2;
};