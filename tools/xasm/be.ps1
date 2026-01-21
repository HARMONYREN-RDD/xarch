tsu build-cpp "g++" "src" "xasmc" "-std=c++23 -O0 -Iinclude -Wall" ;
if ($LASTEXITCODE -eq 0) { .\build\xasmc .\test.xasm }
if ($LASTEXITCODE -eq 0) { xxd -u -c 26 .\out.xb }
