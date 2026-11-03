# sanity
`Sanity` is a small hobbyist operating system. It is built for learning, experimentation and the questionable joy of getting low-level code to run on real hardware.<br>

```
_______________________________________
/ This code makes no sense, but neither \
\ does x86 paging.                      /
---------------------------------------
       \   ^__^
        \  (oo)\_______
           (__)\       )\/\
               ||----w |
               ||     ||
```

## Building
### Nix
If you use Nix with flakes enabled, `flake.nix` provides a dev shell with the whole toolchain:
```bash
nix develop
./setup.sh # needs only to be run once
```