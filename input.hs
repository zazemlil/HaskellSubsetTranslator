v = \(x:xs) -> let { f = \x -> x*x; } in (f x);

v0 :: Int;
v0 = let { f [a, a] = 1; } in (f [True, False]);

v1 :: String;
v2 = let { f a = a; } in (f "asd123");

v3 :: [a];
v3 = let { f (x:xs) = xs; } in (f [1, 2, 3]);

f :: Int -> Int;
f _ = 123;
f 1 = 11;
f 2 = 22;

ff :: (Type1 (Type2 a)) -> a;

ff2 :: (T3 (T4));

ff3 :: (Tree a);

data Maybe a = Just a | Nothing;

func x | x > 0 = 123
    | x < 0 && x / 2 == 0 = -123
    | otherwise = 0;

analyzeGold :: Int -> String;
analyzeGold standard = if {
    | standard == 999 -> "Wow! 999 standard!"
    | standard == 750 -> "Great! 750 standard."
    | standard == 585 -> "Not bad! 585 standard."
    | otherwise -> "I don't know such a standard..."
};

a = ((\x y -> x+y) 15+2*5 3*2);

b = if True then 1 else x+y where { x = 5; y = 4; };

c = \z -> x+y+z where { x = 5; y = 4; };

d = let { z = 3; } in x+y+z where { x = 5; y = 4; };