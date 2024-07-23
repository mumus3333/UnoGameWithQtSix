# Obtener el nombre de la carpeta contenedora
$NombreCarpeta = Split-Path -Leaf (Get-Location)

# Borrar la carpeta contenedora (si existe)
Remove-Item -Path .\$NombreCarpeta -Recurse -Force -ErrorAction SilentlyContinue

# Borrar archivos objetos anteriores
Remove-Item -Path *.o -Force -ErrorAction SilentlyContinue

# Borrar el archivo Makefile
Remove-Item -Path Makefile -Force -ErrorAction SilentlyContinue

# Borrar el archivo .pro si existe
Remove-Item -Path *.pro -Force -ErrorAction SilentlyContinue

# Obtener los archivos de encabezado (.h) y fuentes (.cpp)
$headers = Get-ChildItem -Path . -Filter *.h | ForEach-Object { $_.Name }
$sources = Get-ChildItem -Path . -Filter *.cpp | ForEach-Object { $_.Name }

# Crear el contenido del archivo .pro
$proContent = @"
TEMPLATE = app
TARGET = $($NombreCarpeta)
INCLUDEPATH += .

QT += core gui widgets network

HEADERS += $($headers -join ' ')
SOURCES += $($sources -join ' ')

CONFIG += c++17
"@

# Guardar el archivo .pro
$proFilePath = "$NombreCarpeta.pro"
Set-Content -Path $proFilePath -Value $proContent

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
