clear
f1 = fopen("/home/dmiky/Documenti/lavori_C_Cpp/tesi/arch/axesNodes.dat");
for i=1:100
  x(i)=fscanf(f1,"%f",1);
  y(i)=fscanf(f1,"%f",1);
  z(i)=fscanf(f1,"%f",1);
end
fclose(f1);
f2 = fopen("/home/dmiky/Documenti/lavori_C_Cpp/tesi/arch/IC.dat");
for i=1:100
  for j=1:100
    for k=1:100
      v(j,k,i) = fscanf(f2,"%f",1);
    end
  end
end
fclose(f2);
view(-38,20);
[X,Y,Z] = meshgrid (x,y,z);
figure();
[faces,verts,c] = isosurface (X,Y,Z,v,0.5,Y);
p = patch ("Faces",faces,"Vertices",verts,"FaceVertexCData",c,...
	   "FaceColor","interp","EdgeColor","blue");
set (p, "FaceLighting", "phong");