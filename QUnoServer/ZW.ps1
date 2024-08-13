# Obtener el nombre de la carpeta contenedora
$NombreCarpeta = Split-Path -Leaf (Get-Location)

# Borrar la carpeta contenedora (si existe)
Remove-Item -Path .\$NombreCarpeta -Recurse -Force -ErrorAction SilentlyContinue

# Borrar archivos objetos anteriores
Remove-Item -Path *.o -Force -ErrorAction SilentlyContinue

# Borrar el archivo Makefile
Remove-Item -Path Makefile -Force -ErrorAction SilentlyContinue


# Imprimir información
Write-Output "Generar proyecto para carpeta $NombreCarpeta"

# Generar el archivo Makefile
qmake6 $proFilePath

# Ejecutar el archivo makefile
make

# Ejecutar la aplicación (Siempre y cuando no haya errores)
$exePath = ".\release\$NombreCarpeta.exe"
if (Test-Path $exePath) {
    Start-Process -FilePath $exePath
} else {
    Write-Output "No se encontró el archivo ejecutable en la carpeta release."
}
