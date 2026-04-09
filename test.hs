--x = \x -> \y -> x+y;
x x y = x+y;

--y = \(A (B x) (C y)) -> x+y;
y (A (B x) (C y)) = x+y;

--z = \[x, y, z] -> x+y+z;
z [x, y, z] = x+y+z;

--e = \(x, y, z) -> x+y+z;
e (x, y, z) = x+y+z;

f :: (Int) -> (Int);
f x = let {f :: (Int) -> (Int); f 0 = 1+x; f x = 2+x;} in (f 0);
f2 x = (x 0) where {x :: (Int) -> (Int); x x = x;};

f3 x = case x of {
    0 -> 1;
    n -> n+x;
};

i = f3 + (let {f3 = 123; c = if x then f3 else 0; } in c); 

lc = [(x*f, y*f) | x <- [1*f, 2], y <- [1, 2], let f x = x*x, f < e];

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
--+ нужно переименовывать имена всех локальных определений и их вызовы (... -> Parser -> Renamer -> StaticAnalyze -> ...)
    --+ let 
    --+ where
    --+ patterns
    --+ lambda with pattern
    --+ case 
    --+ global and local def
    -- list comprehension
-- let с аргументами в list comprehension (исправить трансляцию)
-- let в list comprehension с одинаковым id (добавить случай в статический анализ)
-- отрефакторить грамматику (simple type и т.д. убрать)
-- исправить грамматику (добавить поддержку паттернов) для list comprehension
-- добавить ScopeChecker
-- *граф зависимостей в области локальных определений (это модуль оптимизаций, после IRGen + в IRGen нужно все where транслировать в let + Translator будет различать только let и letrec)
-- *в грамматику добавить возможность писать просто выражения (=> если есть взаимная рекурсия глобальных определений, то объединять их в let в выражении где они используются)

-- Вопросы:
-- 1. Было бы удобно использовать case-of вместо FATBAR, т.к. можно тестить в Haskell. Или вообще отказаться от FATBAR?
-- 2. Что делать с АДТ?
-- 3. Как правильно транслировать LETREC с 2 и более локальными определениями (если они, например, связаны взаимной рекурсией)?