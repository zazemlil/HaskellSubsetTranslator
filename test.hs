f0 = [5 |];
f1 :: (Int);
f1 = [5 | (True)];
f2 = [x | let x = 123];
f3 = [x | x <- [1, 2]];
f4 = [x*x | x <- [4, 5], x*x <= 16];
f5 :: (Int);
f5 = [(x, y) | y <- [10, 11, 12], x <- [1, 2, 3]];

concatMap f [] = [];
concatMap f (x:xs) = (f x) ++ (concatMap f xs);  

ff = x where {x = 2;}; 
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
--+ исправить порядок определений (он неправильный из за unordered_map)
-- нужно добавить трансляцию АТД и сигнатур в elc 
-- нужно переименовывать имена всех локальных определений и их вызовы
-- исправить грамматику для list comprehension
-- *граф зависимостей в области локальных определений (это модуль оптимизаций, после IRGen + в IRGen нужно все where транслировать в let + Translator будет различать только let и letrec)
-- *в грамматику добавить возможность писать просто выражения (=> если есть взаимная рекурсия глобальных определений, то объединять их в let в выражении где они используются)