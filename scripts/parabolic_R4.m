function v = parabolic_R4(x,y,z,time)
  tmp = sqrt(x.^2+y.^2+z.^2);
for i=1:55
  for j=1:55
    for k=1:55
      if tmp(j,k,i) <= 4 - 2.0*time
	v(j,k,i) = (4-tmp(i,k,j)^2-2.0*time)^2;
      else
	v(j,k,i) = 0;
      end
    end
  end
end
end