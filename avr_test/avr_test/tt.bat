@echo off
chcp 65001
for %%f in (*.txt) do (
    echo Converting %%f
    type "%%f" > "%%~nf.tmp"
    move /y "%%~nf.tmp" "%%f"
)
echo Conversion complete
pause