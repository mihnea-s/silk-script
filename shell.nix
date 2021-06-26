{ pkgs ? import <nixpkgs> { } }:

pkgs.clangStdenv.mkDerivation {
  name = "silk-script";
  
  nativeBuildInputs = with pkgs; [
    cmake ninja gdb
    clang_11 clang-tools
  ];
}
