{
  description = "Raylib game";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-24.11";

  outputs = inputs@{ self, nixpkgs }: {
    devShells = {
      aarch64-darwin.default = let
          pkgs = nixpkgs.legacyPackages.aarch64-darwin;
        in pkgs.mkShell {
          buildInputs = with pkgs; [
            clang
            lldb
            raylib
          ];
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
            mkdir -p bin
            clang -g -O0 -Wall -Wextra -Werror \
            level.c vector2.c render.c camera.c window.c \
            entity.c entity_physics.c entity_input.c main.c \
            $(pkg-config --cflags --libs raylib) \
            -framework IOKit \
            -framework Cocoa \
            -framework GLUT \
            -framework OpenGL \
            -o raylib-game
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp raylib-game $out/bin
            chmod +x $out/bin/raylib-game
          '';
        };
    };
  };
}
