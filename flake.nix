{
  description = "A Nix-flake-based Node.js development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-24.05";
  };

  outputs = inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [ ];
      systems = [ "x86_64-linux" ];
      perSystem = { config, self', inputs', pkgs, system, ... }: {
        devShells.default = pkgs.mkShell {
          hardeningDisable = [ "fortify" ];
          buildInputs = with pkgs; [
            llvmPackages_18.clang-tools    # libclang from LLVM 1
            llvmPackages_18.libclang    # libclang from LLVM 1
            llvmPackages_18.libllvm
            #llvmPackages_18.clang
            llvmPackages_18.libcxx
            llvmPackages_18.libcxx.dev
            gdb
            gf
            valgrind
            git
            lazygit
            cmake
          ];
          shellHook = ''
            export CXX="${pkgs.gcc14}/bin/g++"
            export CXX_STD_INCLUDE_DIR="${pkgs.gcc14.cc}/include/c++/14.1.0"
            CPLUS_INCLUDE_PATH=""
            CPLUS_INCLUDE_PATH+="${pkgs.gcc14.cc}/include/c++/14.1.0"
            CPLUS_INCLUDE_PATH+=":${pkgs.glibc.dev}/include"
            CPLUS_INCLUDE_PATH+=":${pkgs.gcc14.cc}/include/c++/14.1.0/x86_64-unknown-linux-gnu"
            CPLUS_INCLUDE_PATH+=":${pkgs.gcc14.cc}/lib/gcc/x86_64-unknown-linux-gnu/14.1.0/include"
            export CPLUS_INCLUDE_PATH
          '';
        };
      };
      flake = {
      };
    };
}
