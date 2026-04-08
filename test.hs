x = \x -> \x -> x;

y = \x -> x;

z0 = x where {c = 1; c = 2;};
z1 = let {x = 5; x = 1;} in x;

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
    -- patterns
    -- lambda with pattern
    -- case 
    -- global and local def
-- исправить грамматику (добавить поддержку паттернов) для list comprehension
-- *граф зависимостей в области локальных определений (это модуль оптимизаций, после IRGen + в IRGen нужно все where транслировать в let + Translator будет различать только let и letrec)
-- *в грамматику добавить возможность писать просто выражения (=> если есть взаимная рекурсия глобальных определений, то объединять их в let в выражении где они используются)

-- Вопросы:
-- 1. Было бы удобно использовать case-of вместо FATBAR, т.к. можно тестить в Haskell. Или вообще отказаться от FATBAR?
-- 2. Что делать с АДТ?
-- 3. Как правильно транслировать LETREC с 2 и более локальными определениями (если они, например, связаны взаимной рекурсией)?