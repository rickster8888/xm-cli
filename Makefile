all:
	g++ download.cpp -L ../x64 -lxmnetsdk -I ../include -o ipcamera-download
	g++ diskinfo.cpp -L ../x64 -lxmnetsdk -I ../include -o ipcamera-diskinfo
	g++ timeinfo.cpp -L ../x64 -lxmnetsdk -I ../include -o ipcamera-timeinfo
