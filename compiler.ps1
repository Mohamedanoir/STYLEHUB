# ============================================================
#  Compilation de FashioNova (Windows / Qt 6 / MinGW)
#  Utilisation (PowerShell, dans le dossier FashioNova) :
#     powershell -ExecutionPolicy Bypass -File .\compiler.ps1
#  Résultat : dossier  .\FashioNova_exe\  avec FashioNova.exe prêt à lancer
# ============================================================

$ErrorActionPreference = "Stop"

# --- 1) Trouver Qt (ex : C:\Qt\6.7.3\mingw_64) ---
$qtDir = Get-ChildItem "C:\Qt" -Directory -ErrorAction SilentlyContinue |
         Where-Object { $_.Name -match '^6\.' } | Sort-Object Name -Descending |
         ForEach-Object { Get-ChildItem $_.FullName -Directory | Where-Object { $_.Name -like 'mingw*' } } |
         Select-Object -First 1
if (-not $qtDir) { Write-Host "Qt 6 MinGW introuvable dans C:\Qt" -ForegroundColor Red; exit 1 }
$qt = $qtDir.FullName

# --- 2) Trouver MinGW, CMake et Ninja fournis avec Qt ---
$mingw = (Get-ChildItem "C:\Qt\Tools" -Directory | Where-Object { $_.Name -like 'mingw*_64' } |
          Sort-Object Name -Descending | Select-Object -First 1).FullName
$cmake = "C:\Qt\Tools\CMake_64\bin"
$ninja = "C:\Qt\Tools\Ninja"

Write-Host "Qt    : $qt"
Write-Host "MinGW : $mingw"
$env:PATH = "$qt\bin;$mingw\bin;$cmake;$ninja;$env:PATH"

# --- 3) Configurer + compiler ---
$build = Join-Path $PSScriptRoot "build"
cmake -S $PSScriptRoot -B $build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$qt"
cmake --build $build

# --- 4) Préparer un dossier autonome (exe + DLL Qt) ---
$sortie = Join-Path $PSScriptRoot "FashioNova_exe"
New-Item -ItemType Directory -Force -Path $sortie | Out-Null
Copy-Item "$build\FashioNova.exe" $sortie -Force
windeployqt --release --no-translations "$sortie\FashioNova.exe"

Write-Host ""
Write-Host "Compilation terminee : $sortie\FashioNova.exe" -ForegroundColor Green
Start-Process "$sortie\FashioNova.exe"
