![Preview](images/preview.png)
## Music Player and Visualizer
Music Visualizer written in C for educational purposes using Raylib.

### Self Building

#### Linux 
The `src/`folder contains a Makefile. You can build by using the following command  
```bash
make release
```
#### Windows
The `src/`folder contains a Makefile.  
You need to have mingw installed.  
Mingw can be installed through scoop:  
```bash
scoop install mingw
```
After that you can build it with the following command.
```bash
mingw32-make release
```

### Tested File Types
- MP3
- FLAC (Experimental)

### Hotkeys
`<Space>` Pause/Play  
`<F5>` Replay dropped music  

#### Libraries used:
- [Raylib](https://www.raylib.com/)


#### Resources referenced: 
- [Tsoding's FFT Video](https://youtu.be/Xdbk1Pr5WXU?si=X0qvTVnFx3BDvT0j)
- [Veritasium](https://youtu.be/nmgFG7PUHfo?si=D17flxKJ4SzW73AN)
- [Raylib's examples](https://www.raylib.com/examples.html)
- [RayLib's API reference](https://www.raylib.com/cheatsheet/cheatsheet.html)


