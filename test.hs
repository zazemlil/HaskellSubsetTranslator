mappairs f [] ys = [];
mappairs f (x:xs) [] = [];
mappairs f (x:xs) (y:ys) = (cons (f x y) (mappairs f xs ys));

f [] [] = 1;
f xs ys = 2;