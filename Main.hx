package;

import sys.FileSystem;
import haxe.Timer;
import h2d.Bitmap;
import h2d.Tile;
import h2d.filter.Blur;
import h3d.Engine;


class Main extends hxd.App
{
	/////////////////////////////////////////////////////////////////////////////////////

	static function main()
	{
		new Main();
	}

	/////////////////////////////////////////////////////////////////////////////////////

	var v:VideoBitmap;

	override function init()
	{
		super.init();
		hxd.Res.initEmbed();
		var t:Tile = hxd.Res.pic.toTile();
		var bb:Bitmap = new Bitmap(t,s2d);
		bb.width = 1920;
		bb.height = 1080;
		bb.filter = new Blur(2500);

		var tf = new h2d.Text(hxd.res.DefaultFont.get(), s2d);
		tf.textColor = 0xFFFFFF;
		tf.dropShadow = { dx : 0.5, dy : 0.5, color : 0x555555, alpha : 0.8 };
		tf.text = 'Videotexture using Windows Mediafoundation, DirectX "no-copy" and Heaps';
		tf.x = 100;
		tf.y = 60;

		v = new VideoBitmap();
		s2d.addChild(v);
		v.playVideo("SampleVideo.mp4");
		v.x = 100;
		v.y = 100;

		function onEvent(event : hxd.Event) {
			switch(event.kind) {
				case EKeyDown:
				{
					if (event.keyCode == 39)
						@:privateAccess v.currentTime += 1;
					else if (event.keyCode == 37)
						@:privateAccess v.currentTime -= 1;
				}
				case _:
			}
		}
		hxd.Window.getInstance().addEventTarget(onEvent);
	}

	/////////////////////////////////////////////////////////////////////////////////////	
}
