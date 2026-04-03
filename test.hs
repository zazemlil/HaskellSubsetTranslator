f = "abc123";
x :: (Int);
x = String ['a', 'b', 'c', '1', '2', '3'];

--+ if -> case
--+ List -> :
--+ List pattern -> :
--+ case -> FATBAR
--+ let and where -> application with fix
--+ string -> constructor with list of char arg
--+ char -> constructor with ascii code
-- list comprehension

-- проблема сигнатур в локальных определениях (нужно либо делать аннотации, либо держать сигнатуры отдельно и переименовывать локальные определения)
-- fix нужен не только для let и where, но и в глобальных определениях + нужно использовать fix только по необходимости