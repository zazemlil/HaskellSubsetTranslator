f :: Int -> Int -> Int -> Int;
f z = \x -> \y -> x*y*z;

g = (f (\x -> [1, 2]));

c = (1, 2, 3);

tree = Tree 5 (Leaf 1) (Empty);