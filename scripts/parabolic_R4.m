function f = parabolic_R4(x,y,z)
  tmp = x.^2+y.^2+z.^2;
for i=1:3
  for j=1:3
    for k=1:3
      if sqrt(tmp(j,k,i)) <= 1
	f(j,k,i) = 1-tmp(j,k,i);
      else
	f(j,k,i) = 0.0;
      end
    end
  end
end
end