# Mergery
Simple plugin which allow use UE4 as git merge-tool

## Important notice
Mergery is proof of concent and it not ready for real use

## Build and Install
Clone and build Mergery with RunUAT.
```
\path\to\RunUAT.bat BuildPlugin -Plugin="path\to\cloned\Mergery.uplugin" -Package="\path\to\build\outcome\Mergery"
```

Download and install go from https://golang.org
open a command promt
```
cd \path\to\build\outcome\Mergery\smort-merge
go build
```
It will prodice `smort-merge.exe` which you have to copy to `\path\to\build\outcome\Mergery\Binaries\Win64` or to any other place.

Open `\path\to\build\outcome\Mergery\Mergery.uplugin` with text edior and add `"EnabledByDefault" : true,` to it.

Create `\path\to\UE_4.26\Engine\Plugins\Marketplace` folder, and copy `\path\to\build\outcome\Mergery` there

* You don't have to put Mergery to project `Plugins` folder.
* You don't have to activate it in Plugins windows. Nobody will know that you are using it.

## Configure
Open git bash promt
Use `/` instead of `\`
```bash

git config --global mergetool.smort-merge.path "/path/to/UE_4.26/Engine/Plugins/Marketplace/Mergery/Binaries/Win64/smort-merge.exe"
git config --global mergetool.smort-merge.cmd '/path/to/UE_4.26/Engine/Plugins/Marketplace/Mergery/Binaries/Win64/smort-merge.exe --base "$BASE" --local "$LOCAL" --remote "$REMOTE" --output "$MERGED" -engine "/path/to/UE_4.26/Engine/Binaries/Win64/UE4Editor.exe" -fallback "C:/Program Files/KDiff3/kdiff3.exe"'

git config --global merge.tool smort-merge
git config --global merge.guitool smort-merge
git config --global mergetool.smort-merge.trustExitCode true

```

## Explanation
`smort-merge.exe` is small util which checks file extentsions, runs UE4 and сontrols UE4's exit codes. It may be implemented in any language what you want.

When `smort-merge.exe` open non `.uasset` file, it will run merge-tool from `--fallback` argument (kdiff3 for example)
