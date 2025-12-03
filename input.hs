-- ((1+2*(3+1))/6) || [1, 2, [3, -4.3], (f 1 2 3 4 "123asd123")]
-- (func 1 2 "asd12")
-- 1+4*5 && 1/3+7 || 123
-- (((1+2) + (f 123 "asd321")) / 6)
-- if 1+2 then True else False
-- [x*a | x <- [1, 3], let a = 3.14]
-- let {a = 1; b = let {x=3.14} in x} in a*b
-- let { a = 1; b = 1 && 2 && 3 } in v

-- let {f [1,2] = x; f2 x = x*x; } in (f (f2 12))
-- \(x:xs) -> let { f = \x -> x*x; } in (f x)

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