{
  description = "Sanity devShell!";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-26.05";
  };

  outputs = {nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
  in {
    devShells.${system}.default = pkgs.mkShellNoCC {
      packages = with pkgs; [
        wget
        gnumake
        nasm
        qemu_full
        llvmPackages.clang-unwrapped
        xorriso
      ];
    };
  };
}