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

--+ if -> case
--+ List -> :
--+ List pattern -> :
--+ case -> FATBAR
-- let and where -> application with fix (+check def depend.)
-- list comprehension
-- output spaces for FATBAR