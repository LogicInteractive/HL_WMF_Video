# HL_WMF_Video
VideoTexture Integration for Haxe / Heaps / HL using Windows Media Foundation

![image](https://user-images.githubusercontent.com/1677550/176906184-4c41458c-30a5-499f-874e-1e45806ca867.png)

This project is an experimental native videotexture in Heaps/HL using Windows Mediafoundation - running on DirectX (hldx only).
Its directly using the windows MF API to decode and playback Video - and this is rendered directly to a Heaps texture using HW acceleration (no-copy) - 
meaning there is very little overhead on playback and decoding. Probably the fastest way to do videoplayback on Windows?

And since its using Windows API's there is also no dependencies, no DLL's or anything to include. And should work for 4K video, HDR, Surround etc.

**Windows 8+ only (unfortunately)**
