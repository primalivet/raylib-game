{
  description = "Raylib game";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-24.11";

  outputs = inputs@{ self, nixpkgs }: 
    let
      crossPlatformFlags = ''
        export TARGET_DIR="bin"
        export TARGET_NAME="game"
        export SRC_FILES="level.c vector2.c render.c camera.c window.c entity.c entity_physics.c entity_input.c entity_animation.c entity_bullets.c debug.c main.c"
        export CFLAGS="-g -O0 -Wall -Wextra -Werror $(pkg-config --cflags raylib)"
        export LDFLAGS="$(pkg-config --libs raylib)"
      '';

      darwinFlags = pkgs: ''
        export CC="clang"
        export CFLAGS="$CFLAGS -isystem ${pkgs.clang}/resource-root/include"
        export LDFLAGS="$LDFLAGS -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL"
      '';
    in {
    devShells = {
      aarch64-darwin.default = let
          pkgs = nixpkgs.legacyPackages.aarch64-darwin;
        in pkgs.mkShell {
          buildInputs = with pkgs; [
            pkg-config
            clang
            lldb
            raylib
            gnumake
            bear
          ];

          shellHook = ''
            ${crossPlatformFlags}
            ${darwinFlags pkgs}

            echo "Development shell ready"
            echo "Run 'make' to build the game"
            echo "Run 'make lsp' to generate compile_commands.json for Clangd"
          '';
        };
    };

    packages = {
      aarch64-darwin.default = let
          pkgs = nixpkgs.legacyPackages.aarch64-darwin;
        in pkgs.stdenv.mkDerivation {
          name = "raylib-game";
          src = pkgs.lib.cleanSource ./.;

          nativeBuildInputs = with pkgs; [ clang pkg-config ];
          buildInputs = with pkgs; [ raylib ];

          buildPhase = ''
            ${crossPlatformFlags}
            ${darwinFlags pkgs}
            make clean 
            make
          '';

          installPhase = ''
            mkdir -p $out/$TARGET_DIR
            cp $TARGET_DIR/$TARGET_NAME $out/$TARGET_DIR
            chmod +x $out/$TARGET_DIR/$TARGET_NAME
          '';
        };
    };
  };
}
