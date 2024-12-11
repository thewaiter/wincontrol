# Wincontrol

Wincontrol is a Moksha module for easy window control from a shelf or desktop.


## USAGE

* Icon click: active window close  
* Mouse wheel down: active window iconify  
* Mouse wheel up: toggle normal/maximized window  

## Dependencies

* The usual pkg-config, libtool, intltool,  and meson/ninja
* [EFL](https://www.enlightenment.org/download)
* [Moksha](https://github.com/JeffHoogland/moksha)

## Installation

It is recommended Bodhi users install from Bodhi's repo:

```ShellSession
sudo apt update
sudo apt install moksha-module-wincontrol
```

Other users need to compile the code:

First install all the needed dependencies. Note this includes not only EFL but the EFL header files. If you have compiled and installed EFL, and Moksha from source code this should be no problem. 

Then the usual:

```ShellSession
meson . build
ninja -C build
sudo ninja -C build install
```

## Help wanted

Support for only a few languages are currently provided. The needed PO files have been created but we are requesting users of this modules contribute the needed missing localizations.

Developers may wish to examine our todo file and help implement future features.

Thanks in advance.

## Reporting bugs

Please use the GitHub issue tracker for any bugs or feature suggestions.

## License

This software is released under the same License used in alot of the other Enlightenment projects. It is a custom license but fully Open Source. Please see the included [COPYING](https://github.com/thewaiter/wincontrol/blob/main/COPYING) file and for a less legalese explanation [COPYING-PLAIN](https://github.com/thewaiter/wincontrol/blob/main/COPYING-PLAIN).

Simply put, this software is free to use, modify and redistribute as you see fit. We do ask that you keep the copyright notice the same in any modifications.
