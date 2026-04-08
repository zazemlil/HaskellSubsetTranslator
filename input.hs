mappairs f [] ys = [];
mappairs f (x:xs) [] = [];
mappairs f (x:xs) (y:ys) = (cons (f x y) (mappairs f xs ys));

f1 [] [] = 1;
f1 xs ys = 2;

f11 = \v1 -> \v2 -> case (v1, v2) of {
    ([], []) -> 1;
    (xs, ys) -> 2;
};

foo1 x y = let {
    f3 [] [] = 1;
    f3 xs ys = 2;
} in (f3 x y);

foo11 x y = (f3 x y) where {
    f3 [] [] = 1;
    f3 xs ys = 2;
};

concatMap :: (a -> [b]) -> [a] -> [b];
concatMap f [] = [];
concatMap f (x:xs) = (f x) ++ (concatMap f xs);  

f2 = let {x=2; y = 5 + x;} in x+y; -- не корректная трансляция (нужен граф зависимостей) 
f3 = let {
    even 0 = (True);
    even n = (odd (n-1));

    odd 0 = (False);
    odd n = (even (n-1));
} in (odd 3); -- транслируется корректно, но нужно что то делать с кортежем (он не ленивый в Haskell; можно перед кортежем ставить ленивый паттерн ~)

fact 0 = 1;
fact n = n * (fact (n-1)); -- все ОК