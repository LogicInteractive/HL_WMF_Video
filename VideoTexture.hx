package;

typedef HLExtension = hl.Abstract<"hlmfvideo">;
typedef MPlayer = hl.Abstract<"MediaEnginePlayer*">;

@:hlNative("hlmfvideo")
class VideoTexture extends h3d.mat.Texture
{
	/////////////////////////////////////////////////////////////////////////////////////

	var hasInfo							: Bool;
	var sourceChanged					: Bool;
	var currentSource					: String;
	public var isPlaying				: Bool;
	public var isReady					: Bool;
	public var started					: Bool;
	public var videoWidth				: Int;
	public var videoHeight				: Int;
	public var aspectRatio				: Float			= 1.0;

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

	public var onInfoReady				: (VideoTexture)->Void;
	public var onPlay					: (VideoTexture)->Void;
	public var onProgress				: (VideoTexture)->Void;
	public var onPause					: (VideoTexture)->Void;
	public var onResume					: (VideoTexture)->Void;
	public var onStop					: (VideoTexture)->Void;
	public var onEnded					: (VideoTexture)->Void;

	var mPlayer							: MPlayer;

	/////////////////////////////////////////////////////////////////////////////////////

	public function new(width:Int=1,height:Int=1)
	{
		var engine = h3d.Engine.getCurrent();
		var dxd:h3d.impl.DirectXDriver = cast engine.driver;
		var drv:dx.Driver.DriverInstance = @:privateAccess dxd.driver;
		super(width,height,[Target,IsShared]);
		mPlayer = _create(drv);
	}

	public function render()
	{
		if (!started && !hasInfo && _isInfoReady(mPlayer))
		{
			updateVideoInfo();
		}
		else if (sourceChanged && !hasInfo && _isInfoReady(mPlayer))
		{
			sourceChanged = false;
			updateVideoInfo();
		}

		if (!started && hasInfo && _canPlay(mPlayer))
		{
			_play(mPlayer);
			started = true;
			if (onPlay!=null)
				onPlay(this);
		}

		isPlaying = _isPlaying(mPlayer);
		isEnded = _isEnded(mPlayer);
		isSeeking = _isSeeking(mPlayer);

		if (started && isPlaying)
		{
			@:bypassAccessor currentTime = _getCurrentTime(mPlayer);
			progress = Math.min(currentTime / duration,1.0);
			var tx:dx.Driver.ShaderResourceView = @:privateAccess this.t.view;
			_render(mPlayer,tx);
		}
	}

	function updateVideoInfo()
	{
		videoWidth = _getWidth(mPlayer);
		videoHeight = _getHeight(mPlayer);
		if (videoWidth>0 && videoHeight>0)
		{
			aspectRatio = videoWidth/videoHeight;
			// videoTexture = new h3d.mat.Texture(videoWidth, videoHeight, [Target,IsShared]);
			// videoTexture = new h3d.mat.Texture(1920, 1080, [Target,IsShared]);
			this.resize(videoWidth,videoHeight);
			duration = _getDuration(mPlayer);
			hasAudio = _hasAudio(mPlayer);
			hasVideo = _hasVideo(mPlayer);
			// if (currentSource!=null)
				// _setSource(@:privateAccess currentSource.toUtf8());
			hasInfo = true;
			if (onInfoReady!=null)
				onInfoReady(this);
		}
	}

	public function playVideo(url:String)
	{
		url = haxe.io.Path.normalize(url);
		if (url.indexOf(":/")==-1)
			url = Sys.getCwd()+"/"+url;

		if (url.indexOf(".")==-1)
			return;
		
		var ext:String = haxe.io.Path.extension(url);
		ext = "video/"+ext;
		// if (_canPlayType(mPlayer,@:privateAccess ext.toUtf8()))
		// {
			if (!isReady)
			{	
				currentSource = url;
			}
			hasInfo = false;
			sourceChanged = true;
			_setSource(mPlayer,@:privateAccess url.toUtf8());
		// }
		// else 
		// {
		// 	trace('Error: Cant play file $url');
		// }
	}

	public function stop()
	{
		_stop(mPlayer);
	}

	public function pause()
	{
		_pause(mPlayer);
	}

	public function resume()
	{
		_resume(mPlayer);
	}

	public function isVideoPortait():Bool
	{
		return videoHeight>videoWidth;
	}

	function set_currentTime(value:Float):Float
	{
		_setCurrentTime(mPlayer,value);
		return currentTime=value;
	}

	function set_progress(value:Float):Float
	{
		if (value<0)
			value = 0;
		return currentTime=duration*Math.min(value,1);
	}

	function get_volume():Float
	{
		volume = _getVolume(mPlayer);
		return volume;
	}
	function set_volume(value:Float):Float
	{
		_setVolume(mPlayer,value);
		return volume=value;
	}

	function get_mute():Bool
	{
		mute = _getMuted(mPlayer);
		return mute;
	}
	function set_mute(value:Bool):Bool
	{
		_setMuted(mPlayer,value);
		return mute=value;
	}

	function get_loop():Bool
	{
		loop = _getLoop(mPlayer);
		return loop;
	}
	function set_loop(value:Bool):Bool
	{
		_setLoop(mPlayer,value);
		return loop=value;
	}

	public function destroy()
	{
		_dispose(mPlayer);
		mPlayer = null;
		hasInfo = false;
		currentSource = null;
		isPlaying = false;
		isReady = false;
		started = false;
		onInfoReady = null;
		onPlay = null;
		onPause = null;
		onProgress = null;
		onResume = null;
		onEnded = null;
		dispose();
	}

	/////////////////////////////////////////////////////////////////////////////////////

	@:hlNative("hlmfvideo","hlmf_create")				static function _create(device:dx.Driver.DriverInstance):MPlayer { return null; }
	@:hlNative("hlmfvideo","hlmf_render")				static function _render(mplayer:MPlayer,tx:dx.Driver.ShaderResourceView) { }
	@:hlNative("hlmfvideo","hlmf_dispose")				static function _dispose(mplayer:MPlayer) { }
	@:hlNative("hlmfvideo","hlmf_setSource")			static function _setSource(mplayer:MPlayer,url:hl.Bytes) { }
	@:hlNative("hlmfvideo","hlmf_play")					static function _play(mplayer:MPlayer) { }
	@:hlNative("hlmfvideo","hlmf_stop")					static function _stop(mplayer:MPlayer) { }
	@:hlNative("hlmfvideo","hlmf_pause")				static function _pause(mplayer:MPlayer) { }
	@:hlNative("hlmfvideo","hlmf_resume")				static function _resume(mplayer:MPlayer) { }
	@:hlNative("hlmfvideo","hlmf_canPlay")				static function _canPlay(mplayer:MPlayer):Bool { return false; }
	@:hlNative("hlmfvideo","hlmf_getWidth")				static function _getWidth(mplayer:MPlayer):Int { return 0; }
	@:hlNative("hlmfvideo","hlmf_getHeight")			static function _getHeight(mplayer:MPlayer):Int { return 0; }
	@:hlNative("hlmfvideo","hlmf_getVolume")			static function _getVolume(mplayer:MPlayer):hl.F64 { return 0; }
	@:hlNative("hlmfvideo","hlmf_setVolume")			static function _setVolume(mplayer:MPlayer,volume:hl.F64) { }
	@:hlNative("hlmfvideo","hlmf_getMuted")				static function _getMuted(mplayer:MPlayer):Bool { return false; }
	@:hlNative("hlmfvideo","hlmf_setMuted")				static function _setMuted(mplayer:MPlayer,muted:Bool) { }
	@:hlNative("hlmfvideo","hlmf_getLoop")				static function _getLoop(mplayer:MPlayer):Bool { return false; }
	@:hlNative("hlmfvideo","hlmf_setLoop")				static function _setLoop(mplayer:MPlayer,loop:Bool) { }
	@:hlNative("hlmfvideo","hlmf_getCurrentTime")		static function _getCurrentTime(mplayer:MPlayer):hl.F64 { return 0; }
	@:hlNative("hlmfvideo","hlmf_setCurrentTime")		static function _setCurrentTime(mplayer:MPlayer,seekTime:hl.F64) { }
	@:hlNative("hlmfvideo","hlmf_getDuration")			static function _getDuration(mplayer:MPlayer):hl.F64 { return 0; }
	@:hlNative("hlmfvideo","hlmf_isPlaying")			static function _isPlaying(mplayer:MPlayer):Bool { return false; }
	@:hlNative("hlmfvideo","hlmf_isInfoReady")			static function _isInfoReady(mplayer:MPlayer):Bool { return false; }
	@:hlNative("hlmfvideo","hlmf_hasAudio")				static function _hasAudio(mplayer:MPlayer):Bool { return false; }
	@:hlNative("hlmfvideo","hlmf_hasVideo")				static function _hasVideo(mplayer:MPlayer):Bool { return false; }
	@:hlNative("hlmfvideo","hlmf_isEnded")				static function _isEnded(mplayer:MPlayer):Bool { return false; }
	@:hlNative("hlmfvideo","hlmf_isSeeking")			static function _isSeeking(mplayer:MPlayer):Bool { return false; }
	@:hlNative("hlmfvideo","hlmf_canPlayType")			static function _canPlayType(mplayer:MPlayer,type:hl.Bytes):Bool { return false; }

	/////////////////////////////////////////////////////////////////////////////////////
}
