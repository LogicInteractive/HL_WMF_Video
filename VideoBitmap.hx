package;

import h2d.RenderContext;

class VideoBitmap extends h2d.Bitmap
{
	/////////////////////////////////////////////////////////////////////////////////////

	public var videoTexture				: VideoTexture;

	public var progress(default,set)	: Float;
	public var currentTime(default,set)	: Float;
	public var duration(default,null)	: Float;
	public var hasAudio(default,null)	: Bool;
	public var hasVideo(default,null)	: Bool;
	public var isEnded(default,null)	: Bool;
	public var isSeeking(default,null)	: Bool;

	@:isVar
	public var volume(get,set)			: Float			= 1.0;
	@:isVar
	public var mute(get,set)			: Bool;
	@:isVar
	public var loop(get,set)			: Bool;

	/////////////////////////////////////////////////////////////////////////////////////

	public function new(?source:String)
	{
		super();
		videoTexture = new VideoTexture();
		videoTexture.onInfoReady=onInfoReadyEvent;
		tile = h2d.Tile.fromTexture(videoTexture);
		this.smooth = true;
		if (source!=null)
			playVideo(source);
	}

	override function draw( ctx:RenderContext )
	{
		if (videoTexture!=null)
			videoTexture.render();
		super.draw(ctx);
	}

	function onInfoReadyEvent(v)
	{
		tile.setSize(videoTexture.width,videoTexture.height);
	}

	public function playVideo(url:String)
	{
		videoTexture.playVideo(url);
	}

	public function stop()
	{
		videoTexture.stop();
	}

	public function pause()
	{
		videoTexture.pause();
	}

	public function resume()
	{
		videoTexture.resume();
	}

	function set_currentTime(value:Float):Float
	{
		if (videoTexture!=null)
			videoTexture.currentTime = value;
		return currentTime=value;
	}

	function get_progress(value:Float):Float
	{
		if (videoTexture!=null)
			return progress=videoTexture.progress;
		else 
			return progress;
	}

	function set_progress(value:Float):Float
	{
		progress=value;
		if (videoTexture!=null)
			return videoTexture.progress=value;
		else
			return progress;
	}

	function get_volume():Float
	{
		if (videoTexture!=null)
			volume = videoTexture.volume;
		return volume;
	}
	function set_volume(value:Float):Float
	{
		if (videoTexture!=null)		
			videoTexture.volume = value;
		return volume=value;
	}

	function get_mute():Bool
	{
		if (videoTexture!=null)
			mute = videoTexture.mute;
		return mute;
	}
	function set_mute(value:Bool):Bool
	{
		if (videoTexture!=null)		
			videoTexture.mute = value;
		return mute=value;
	}

	function get_loop():Bool
	{
		if (videoTexture!=null)		
			loop = videoTexture.loop;
		return loop;
	}
	function set_loop(value:Bool):Bool
	{
		if (videoTexture!=null)
			videoTexture.loop = value;
		return loop=value;
	}

	public function dispose()
	{
		if (videoTexture!=null)
		{
			videoTexture.destroy();
			videoTexture = null;
		}
	
		if (this.parent!=null)
			this.parent = null;
	}

	/////////////////////////////////////////////////////////////////////////////////////
}
