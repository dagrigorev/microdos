param(
    [string] $Generator = "Ninja",
    [ValidateSet("Debug", "Release", "DEBUG", "RELEASE")]
    [string] $Configuration = "Debug"
)

$ErrorActionPreference = "Stop"
$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$buildDir = Join-Path $repoRoot "build\host-check"
$config = $Configuration.ToUpperInvariant()

cmake -S $repoRoot -B $buildDir -G $Generator -DCMAKE_BUILD_TYPE=$config
cmake --build $buildDir --config $config
