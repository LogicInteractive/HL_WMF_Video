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
	var tx:h3d.mat.Texture;
	var b:h2d.Bitmap;
	var files:Array<String>;
	var dir:String;

	override function init()
	{
		super.init();
		// engine.backgroundColor = null; // prevent clear bg		
		// var g = new h2d.Graphics(s2d);
		// g.beginFill(0x0000FF, 0.5);
		// g.lineStyle(3, 0x5555FF);
		// g.drawCircle(100, 100, 700);
		// g.beginFill(0x00FF00, 0.5);
		// g.lineStyle(1.3, 0x55FF55, 0.3);
		// g.drawRect(30,30,1600,300);
		// g.endFill();

		// hxd.Res.initEmbed();
		// var t:Tile = hxd.Res.pic.toTile();
		// var bb:Bitmap = new Bitmap(t,s2d);
		// bb.width = 1920;
		// bb.height = 1080;
		// bb.filter = new Blur(2500);

		// var tf = new h2d.Text(hxd.res.DefaultFont.get(), s2d);
		// tf.textColor = 0xFFFFFF;
		// tf.dropShadow = { dx : 0.5, dy : 0.5, color : 0x555555, alpha : 0.8 };
		// tf.text = 'Videotexture using Windows Mediafoundation, DirectX "no-copy" and Heaps';

		// tf.x = 100;
		// tf.y = 60;
		// tf.scale(1);		

		dir = Sys.getCwd();
		files = [];
		for (f in FileSystem.readDirectory(dir))
		{
			var fl = f.toLowerCase();
			if (fl.indexOf(".mp4")>-1 || fl.indexOf(".mkv")>-1 || fl.indexOf(".avi")>-1 || fl.indexOf(".wmv")>-1 || fl.indexOf(".mpg")>-1 )
				files.push(f);
		}

		v = new VideoBitmap();
		s2d.addChild(v);
		// playVid(0);
		v.playVideo("D:/Media/Serier/Foundation (2021)/foundation.s01e01.hdr.2160p.web.h265-ggez.mkv");
		// v.alpha = 0.85;
		// v.x = 100;
		// v.y = 100;
		// v.play("D:/SampleVideo.mp4");
		// v.playVideo("SampleVideo.mp4");
		// v.playVideo("SampleVideo.mp4");
		// v.play("C:/dev/Tools/video/test/bbb.mp4");
		// v.play("D:/dropbox/div/vid_tests/movie4k.mp4");
		// v.play("E:/tmp/vegafilmer1/VikingFight.mp4");
		// v.play("D:/Downloads/samsung.ts");

		// var i = new h2d.Interactive(1280, 720, v);
		// i.onPush = function(event : hxd.Event)
		// {
		// 	// v.pause();
		// }
		// i.onRelease = function(event : hxd.Event)
		// {
		// 	// v.resume();
		// }
		// i.onClick = function(event : hxd.Event)
		// {
			// trace("click!");
			if (v==null)
				return;
			// v.dispose();
			// v = null;
			// haxe.Timer.delay(()->{
				// v = new VideoBitmap();
				// s2d.addChild(v);
				// v.playVideo("C:/dev/Tools/video/test/bbb.mp4");
			// },500);
		// }		


		function onEvent(event : hxd.Event) {
			switch(event.kind) {
				case EKeyDown:
				{
					if (event.keyCode == 39)
						@:privateAccess v.currentTime += 5;
						// @:privateAccess v.progress +=0.1;
					else if (event.keyCode == 37)
						@:privateAccess v.currentTime -= 5;
						// @:privateAccess v.progress -=0.1;
					else if (event.keyCode == 38)
					{
						if (vidIndex>0)
						{	
							vidIndex--;
							playVid(vidIndex);
						}
					}
					else if (event.keyCode == 40)
					{
						if (vidIndex<files.length-1)
						{	
							vidIndex++;
							playVid(vidIndex);
						}
					}
				}
				case _:
			}
		}
		hxd.Window.getInstance().addEventTarget(onEvent);


/* 		var v2 = new VideoBitmap();
		s2d.addChild(v2);
		v2.x = 1400;
		v2.y = 100;
		v2.setScale(0.5);
		v2.playVideo("E:/tmp/vegafilmer1/VikingFight.mp4"); */
	}

	var vidIndex = 0;
	function playVid(ind:Int)
	{
		if (files!=null && files.length>ind)
		{
			vidIndex=ind;
			v.playVideo(dir+files[vidIndex]);
		}
	}

	override function update(dt:Float)
	{
		super.update(dt);
		// v.rotation+=0.0001;
		if (v.videoTexture.isVideoPortait())
		{
			v.height = hxd.Window.getInstance().height;
			v.width = v.height*(1/v.videoTexture.aspectRatio);
		}
		else 
		{
			v.width = hxd.Window.getInstance().width;
			v.height = v.width*(1/v.videoTexture.aspectRatio);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////	
}
