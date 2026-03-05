f1 x = 11;
f2 0 = 22;
f3 _ = 33;
f4 (Just x) = 44;
f5 [] = 55;
f6 [x, 3] = 66;
f7 (x:xs) = 77;

f [] x = 0;
f (y:ys) x = 1;
f z x = 2;

ff (Car (Wheel (Screw 4))) = 0;
ff (Car (Wheel (Screw 5))) = 1;
ff x = 2;

fff (True) = 1;
fff (False) = 2;
fff x = 3;