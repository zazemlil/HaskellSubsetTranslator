f0 = [5 |];
f1 = [5 | (True)];
f2 = [x | let x = 123];
f3 = [x | x <- [1, 2]];
f4 = [x*x | x <- [4, 5], x*x <= 16];
f5 = [(x, y) | y <- [10, 11, 12], x <- [1, 2, 3]];

concatMap f [] = [];
concatMap f (x:xs) = (f x) ++ (concatMap f xs);  

f = let {pi = 3.14;x=x+1;} in pi*2;

--+ if -> case
--+ List -> :
--+ List pattern -> :
--+ case -> FATBAR
--+ let and where -> application with fix
--+ string -> constructor with list of char arg
--+ char -> constructor with ascii code
--+ list comprehension
--+ operator (++)

--+ fix по необходимости не только для let и where, но и в глобальных определениях
-- нужно добавить трансляцию АТД и сигнатур в elc 
-- исправить порядок определений (он неправильный из за unordered_map)
-- исправить грамматику для list comprehension
-- нужно переименовывать имена всех локальных определений и их вызовы
-- *граф зависимостей в области локальных определений