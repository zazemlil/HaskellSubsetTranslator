f0 = [5 |];
f1 = [5 | (True)];
f2 = [x | let x = 123];
f3 = [x | x <- [1, 2]];
f4 = [x*x | x <- [4, 5], x*x <= 16];

--+ if -> case
--+ List -> :
--+ List pattern -> :
--+ case -> FATBAR
--+ let and where -> application with fix
--+ string -> constructor with list of char arg
--+ char -> constructor with ascii code
--+ list comprehension

-- проблема сигнатур в локальных определениях (нужно либо делать аннотации, либо держать сигнатуры отдельно и переименовывать локальные определения)
-- fix нужен не только для let и where, но и в глобальных определениях + нужно использовать fix только по необходимости