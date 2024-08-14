# Obtener el nombre de la carpeta contenedora
NombreCarpeta=`basename $PWD`
# Obtener el nombre de la carpeta contenedora
rm ./$NombreCarpeta
# Borrar programas objetos anteriores
rm *.o
# Borrar el archivo Makefile
rm Makefile 
# Imprimir informacion
echo "Generar proyecto para carpeta $NombreCarpeta"
breCarpeta.pro 
echo "LIBS += -lGL -lGLU -lglut" >> $NombreCarpeta.pro 
# Hacer QMAKE para generar el archivo Makefile
qmake6 
# Ejecutar el archivo makefile
make
# Ejecutar la aplicacion (Siempre y cuando no haya errores)
./$NombreCarpeta
