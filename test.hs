c = (1, 2, 3);

tree = Tree 5 (Leaf 1) (Empty);

e = ((\a -> if a < 5 then 1 else 2) 6);

e2 = [1, 3, 5, 7];
e3 = 1 : 2 : 3 : [];

e4 (x:xs) = x + 5;
e5 [x, 1, _] = x;

f [] [] = 1;
f xs ys = 2;

fact 0 = 1;
fact n = n * (fact (n-1));

mappairs f [] ys = [];
mappairs f xs [] = [];
mappairs f (x:xs) (y:ys) = ((f x y) : (mappairs f xs ys));

e6 = let {x :: Int; y :: Int; x = 5; y = 3;} in x+y;
e7 = z/pi where {z = 1; pi = 3.14;};
e8 = let {pi x = 3.14*x;} in (pi 2);

--+ if -> case
--+ List -> :
--+ List pattern -> :
--+ case -> FATBAR
--+ let and where -> application with fix
-- string -> constructor with list of char arg
-- char -> constructor with ascii code
-- list comprehension

-- проблема сигнатур в локальных определениях (нужно либо делать аннотации, либо держать сигнатуры отдельно и переименовывать локальные определения)
-- fix нужен не только для let и where, но и в глобальных определениях + нужно использовать fix только по необходимости