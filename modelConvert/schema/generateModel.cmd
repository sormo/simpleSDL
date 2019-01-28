call flatc.exe --json --cpp --gen-object-api model.fbs
move /Y *.h ..\include

:: to json
:: flatc.exe --json --raw-binary model.fbs -- nanosuit.model
:: from json
:: flatc.exe --binary model.fbs nanosuit.json